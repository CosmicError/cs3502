using System;
using System.IO;
using System.Windows.Forms;
using CS3502_Project3.Core;

namespace CS3502_Project3
{
    public partial class Form1 : Form
    {
        private FileOperations fileOps;
        private DirectoryOperations dirOps;
        private string currentPath;

        // GUI Components
        private TextBox txtCurrentPath;
        private ListBox lstFiles;
        private TextBox txtFileContent;
        private Button btnCreate;
        private Button btnRead;
        private Button btnUpdate;
        private Button btnDelete;
        private Button btnRename;
        private Button btnCreateDir;
        private Button btnNavigateUp;
        private Label lblStatus;

        public Form1()
        {
            InitializeComponent(); // Calls the Designer's method
            InitializeCustomControls(); // Our custom controls

            fileOps = new FileOperations();
            dirOps = new DirectoryOperations();
            currentPath = Environment.GetFolderPath(
                Environment.SpecialFolder.MyDocuments
            );

            RefreshFileList();
        }

        private void InitializeCustomControls()
        {
            this.Text = "File Manager - CS 3502 Project 3";
            this.Size = new System.Drawing.Size(800, 600);
            this.StartPosition = FormStartPosition.CenterScreen;

            // Current Path Display
            txtCurrentPath = new TextBox
            {
                Location = new System.Drawing.Point(10, 10),
                Size = new System.Drawing.Size(700, 25),
                ReadOnly = true,
                BackColor = System.Drawing.Color.LightGray
            };
            this.Controls.Add(txtCurrentPath);

            // Navigate Up Button
            btnNavigateUp = new Button
            {
                Text = "⬆ Up",
                Location = new System.Drawing.Point(720, 10),
                Size = new System.Drawing.Size(60, 25)
            };
            btnNavigateUp.Click += BtnNavigateUp_Click;
            this.Controls.Add(btnNavigateUp);

            // File List
            lstFiles = new ListBox
            {
                Location = new System.Drawing.Point(10, 45),
                Size = new System.Drawing.Size(350, 400),
                Font = new System.Drawing.Font(
                    "Consolas",
                    9F,
                    System.Drawing.FontStyle.Regular
                )
            };
            lstFiles.DoubleClick += LstFiles_DoubleClick;
            this.Controls.Add(lstFiles);

            // File Content Area
            txtFileContent = new TextBox
            {
                Location = new System.Drawing.Point(370, 45),
                Size = new System.Drawing.Size(410, 400),
                Multiline = true,
                ScrollBars = ScrollBars.Both,
                Font = new System.Drawing.Font(
                    "Consolas",
                    9F,
                    System.Drawing.FontStyle.Regular
                )
            };
            this.Controls.Add(txtFileContent);

            // Buttons
            int btnY = 455;
            int btnX = 10;
            int btnWidth = 100;
            int btnSpacing = 110;

            btnCreate = CreateButton("Create File", btnX, btnY, btnWidth);
            btnCreate.Click += BtnCreate_Click;

            btnRead = CreateButton(
                "Read File",
                btnX + btnSpacing,
                btnY,
                btnWidth
            );
            btnRead.Click += BtnRead_Click;

            btnUpdate = CreateButton(
                "Update File",
                btnX + btnSpacing * 2,
                btnY,
                btnWidth
            );
            btnUpdate.Click += BtnUpdate_Click;

            btnDelete = CreateButton(
                "Delete",
                btnX + btnSpacing * 3,
                btnY,
                btnWidth
            );
            btnDelete.Click += BtnDelete_Click;

            btnRename = CreateButton(
                "Rename",
                btnX + btnSpacing * 4,
                btnY,
                btnWidth
            );
            btnRename.Click += BtnRename_Click;

            btnCreateDir = CreateButton(
                "New Folder",
                btnX + btnSpacing * 5,
                btnY,
                btnWidth
            );
            btnCreateDir.Click += BtnCreateDir_Click;

            // Status Label
            lblStatus = new Label
            {
                Location = new System.Drawing.Point(10, 495),
                Size = new System.Drawing.Size(770, 50),
                BorderStyle = BorderStyle.FixedSingle,
                Text = "Ready - Double-click folders to navigate",
                Padding = new Padding(5),
                BackColor = System.Drawing.Color.LightYellow
            };
            this.Controls.Add(lblStatus);
        }

        private Button CreateButton(
            string text,
            int x,
            int y,
            int width
        )
        {
            var btn = new Button
            {
                Text = text,
                Location = new System.Drawing.Point(x, y),
                Size = new System.Drawing.Size(width, 30)
            };
            this.Controls.Add(btn);
            return btn;
        }

        private void RefreshFileList()
        {
            lstFiles.Items.Clear();
            txtCurrentPath.Text = currentPath;

            var items = dirOps.ListDirectory(currentPath, out string error);

            if (!string.IsNullOrEmpty(error))
            {
                ShowStatus($"Error: {error}", true);
                return;
            }

            foreach (var item in items)
            {
                string displayName = Path.GetFileName(item);
                if (string.IsNullOrEmpty(displayName))
                    continue;

                if (Directory.Exists(item))
                {
                    displayName = "[DIR] " + displayName;
                }
                else
                {
                    displayName = "[FILE] " + displayName;
                }
                lstFiles.Items.Add(displayName);
            }

            ShowStatus($"Loaded {items.Count} items");
        }

        private void LstFiles_DoubleClick(object sender, EventArgs e)
        {
            if (lstFiles.SelectedItem == null) return;

            string selected = lstFiles.SelectedItem.ToString();
            selected = selected.Replace("[DIR] ", "").Replace("[FILE] ", "");

            string fullPath = Path.Combine(currentPath, selected);

            if (Directory.Exists(fullPath))
            {
                currentPath = fullPath;
                RefreshFileList();
                txtFileContent.Clear();
            }
        }

        private void BtnNavigateUp_Click(object sender, EventArgs e)
        {
            DirectoryInfo parent = Directory.GetParent(currentPath);
            if (parent != null)
            {
                currentPath = parent.FullName;
                RefreshFileList();
                txtFileContent.Clear();
            }
            else
            {
                ShowStatus("Already at root directory", true);
            }
        }

        private void BtnCreate_Click(object sender, EventArgs e)
        {
            string fileName = PromptForInput(
                "Create File",
                "Enter file name (with extension):"
            );
            if (string.IsNullOrEmpty(fileName)) return;

            string fullPath = Path.Combine(currentPath, fileName);
            string content = txtFileContent.Text;

            if (fileOps.CreateFile(fullPath, content, out string error))
            {
                ShowStatus($"✓ File created: {fileName}");
                RefreshFileList();
                txtFileContent.Clear();
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private void BtnRead_Click(object sender, EventArgs e)
        {
            if (lstFiles.SelectedItem == null)
            {
                ShowStatus("Please select a file", true);
                return;
            }

            string selected = lstFiles.SelectedItem.ToString()
                .Replace("[DIR] ", "").Replace("[FILE] ", "");
            string fullPath = Path.Combine(currentPath, selected);

            if (Directory.Exists(fullPath))
            {
                ShowStatus("Cannot read a directory", true);
                return;
            }

            string content = fileOps.ReadFile(fullPath, out string error);

            if (content != null)
            {
                txtFileContent.Text = content;
                ShowStatus($"✓ File loaded: {selected}");
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private void BtnUpdate_Click(object sender, EventArgs e)
        {
            if (lstFiles.SelectedItem == null)
            {
                ShowStatus("Please select a file", true);
                return;
            }

            string selected = lstFiles.SelectedItem.ToString()
                .Replace("[DIR] ", "").Replace("[FILE] ", "");
            string fullPath = Path.Combine(currentPath, selected);

            if (Directory.Exists(fullPath))
            {
                ShowStatus("Cannot update a directory", true);
                return;
            }

            if (fileOps.UpdateFile(
                fullPath,
                txtFileContent.Text,
                out string error
            ))
            {
                ShowStatus($"✓ File updated: {selected}");
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private void BtnDelete_Click(object sender, EventArgs e)
        {
            if (lstFiles.SelectedItem == null)
            {
                ShowStatus("Please select an item", true);
                return;
            }

            string selected = lstFiles.SelectedItem.ToString()
                .Replace("[DIR] ", "").Replace("[FILE] ", "");
            string fullPath = Path.Combine(currentPath, selected);

            var result = MessageBox.Show(
                $"Are you sure you want to delete:\n\n{selected}?",
                "Confirm Delete",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Warning
            );

            if (result != DialogResult.Yes) return;

            bool success;
            string error;

            if (Directory.Exists(fullPath))
            {
                success = dirOps.DeleteDirectory(fullPath, true, out error);
            }
            else
            {
                success = fileOps.DeleteFile(fullPath, out error);
            }

            if (success)
            {
                ShowStatus($"✓ Deleted: {selected}");
                RefreshFileList();
                txtFileContent.Clear();
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private void BtnRename_Click(object sender, EventArgs e)
        {
            if (lstFiles.SelectedItem == null)
            {
                ShowStatus("Please select an item", true);
                return;
            }

            string selected = lstFiles.SelectedItem.ToString()
                .Replace("[DIR] ", "").Replace("[FILE] ", "");
            string oldPath = Path.Combine(currentPath, selected);

            string newName = PromptForInput(
                "Rename",
                "Enter new name:",
                selected
            );
            if (string.IsNullOrEmpty(newName)) return;

            string newPath = Path.Combine(currentPath, newName);

            if (fileOps.RenameFile(oldPath, newPath, out string error))
            {
                ShowStatus($"✓ Renamed to: {newName}");
                RefreshFileList();
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private void BtnCreateDir_Click(object sender, EventArgs e)
        {
            string dirName = PromptForInput(
                "Create Folder",
                "Enter folder name:"
            );
            if (string.IsNullOrEmpty(dirName)) return;

            string fullPath = Path.Combine(currentPath, dirName);

            if (dirOps.CreateDirectory(fullPath, out string error))
            {
                ShowStatus($"✓ Folder created: {dirName}");
                RefreshFileList();
            }
            else
            {
                ShowStatus($"✗ Error: {error}", true);
            }
        }

        private string PromptForInput(
            string title,
            string prompt,
            string defaultValue = ""
        )
        {
            Form promptForm = new Form
            {
                Width = 400,
                Height = 150,
                Text = title,
                StartPosition = FormStartPosition.CenterParent,
                FormBorderStyle = FormBorderStyle.FixedDialog,
                MaximizeBox = false,
                MinimizeBox = false
            };

            Label lblPrompt = new Label
            {
                Left = 10,
                Top = 10,
                Text = prompt,
                Width = 360
            };

            TextBox txtInput = new TextBox
            {
                Left = 10,
                Top = 40,
                Width = 360,
                Text = defaultValue
            };

            Button btnOk = new Button
            {
                Text = "OK",
                Left = 200,
                Top = 70,
                Width = 80,
                DialogResult = DialogResult.OK
            };

            Button btnCancel = new Button
            {
                Text = "Cancel",
                Left = 290,
                Top = 70,
                Width = 80,
                DialogResult = DialogResult.Cancel
            };

            promptForm.Controls.Add(lblPrompt);
            promptForm.Controls.Add(txtInput);
            promptForm.Controls.Add(btnOk);
            promptForm.Controls.Add(btnCancel);
            promptForm.AcceptButton = btnOk;
            promptForm.CancelButton = btnCancel;

            return promptForm.ShowDialog() == DialogResult.OK
                ? txtInput.Text
                : string.Empty;
        }

        private void ShowStatus(string message, bool isError = false)
        {
            lblStatus.Text = message;
            lblStatus.ForeColor = isError
                ? System.Drawing.Color.Red
                : System.Drawing.Color.DarkGreen;
        }
    }
}