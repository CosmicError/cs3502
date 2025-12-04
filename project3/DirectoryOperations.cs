using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace CS3502_Project3.Core
{
    public class DirectoryOperations
    {
        // Create directory
        public bool CreateDirectory(string path, out string error)
        {
            error = string.Empty;

            try
            {
                if (Directory.Exists(path))
                {
                    error = "Directory already exists";
                    return false;
                }

                Directory.CreateDirectory(path);
                return true;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }

        // Delete directory
        public bool DeleteDirectory(
            string path,
            bool recursive,
            out string error
        )
        {
            error = string.Empty;

            try
            {
                if (!Directory.Exists(path))
                {
                    error = "Directory not found";
                    return false;
                }

                Directory.Delete(path, recursive);
                return true;
            }
            catch (IOException ex)
            {
                error = $"Directory may not be empty: {ex.Message}";
                return false;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return false;
            }
        }

        // List directory contents
        public List<string> ListDirectory(string path, out string error)
        {
            error = string.Empty;

            try
            {
                var files = Directory.GetFiles(path).ToList();
                var directories = Directory.GetDirectories(path).ToList();

                var items = new List<string>();
                items.AddRange(directories);
                items.AddRange(files);

                return items;
            }
            catch (Exception ex)
            {
                error = $"Error: {ex.Message}";
                return new List<string>();
            }
        }
    }
}