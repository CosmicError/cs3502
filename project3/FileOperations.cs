using System;
using System.IO;

namespace CS3502_Project3.Core
{
    public class FileOperations
    {
        // CREATE: Create a new file
        public bool CreateFile(string path, string content, out string error)
        {
            error = string.Empty;

            try
            {
                if (File.Exists(path))
                {
                    error = "File already exists";
                    return false;
                }

                File.WriteAllText(path, content);
                return true;
            }
            catch (UnauthorizedAccessException)
            {
                error = "Permission denied";
                return false;
            }
            catch (IOException ex)
            {
                error = $"I/O Error: {ex.Message}";
                return false;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }

        // READ: Read file contents
        public string ReadFile(string path, out string error)
        {
            error = string.Empty;

            try
            {
                if (!File.Exists(path))
                {
                    error = "File not found";
                    return null;
                }

                return File.ReadAllText(path);
            }
            catch (UnauthorizedAccessException)
            {
                error = "Permission denied";
                return null;
            }
            catch (Exception ex)
            {
                error = $"Error reading file: {ex.Message}";
                return null;
            }
        }

        // UPDATE: Update file contents
        public bool UpdateFile(string path, string newContent, out string error)
        {
            error = string.Empty;

            try
            {
                if (!File.Exists(path))
                {
                    error = "File not found";
                    return false;
                }

                File.WriteAllText(path, newContent);
                return true;
            }
            catch (UnauthorizedAccessException)
            {
                error = "Permission denied";
                return false;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }

        // DELETE: Delete a file
        public bool DeleteFile(string path, out string error)
        {
            error = string.Empty;

            try
            {
                if (!File.Exists(path))
                {
                    error = "File not found";
                    return false;
                }

                File.Delete(path);
                return true;
            }
            catch (UnauthorizedAccessException)
            {
                error = "Permission denied";
                return false;
            }
            catch (IOException ex)
            {
                error = $"File may be in use: {ex.Message}";
                return false;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }

        // RENAME: Rename a file
        public bool RenameFile(string oldPath, string newPath, out string error)
        {
            error = string.Empty;

            try
            {
                if (!File.Exists(oldPath))
                {
                    error = "File not found";
                    return false;
                }

                if (File.Exists(newPath))
                {
                    error = "A file with that name already exists";
                    return false;
                }

                File.Move(oldPath, newPath);
                return true;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }
    }
}