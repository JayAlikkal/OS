#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "test.h"

int main(int argc, char *argv[])
{
    filetype entry;
    FILE *fptr;
    fptr = fopen("Directory.dat", "rb");

    if (fptr == NULL)
    {
        fprintf(stderr, "Error: cannot open file %s\n", argv[1]);
        exit(1);
    }

    int bytes_size = 0;
    int longfile_count = 0;
    int fcounter = 0;
    int dircount = 0;

    while (fread(&entry, sizeof(fat32_shortfile), 1, fptr) != 0)
    {
        if ((entry.sfile.attributes & 0x18) == 0x18 || (entry.sfile.filename[0] == 0xE5))
        {
            continue;
        }

        fat32_Longfile long_entry[11];

        if (entry.sfile.attributes == 0x0F)
        {
            // Add LFN entry to struct array
            long_entry[longfile_count] = entry.lfile;
            longfile_count++;
        }
        else
        {
            if ((entry.sfile.attributes & 0x02) == 0x02) // Check if the entry is a hidden directory or file and reset the LFN entries if it is
            {
                memset(long_entry, 0, 11 * sizeof(fat32_Longfile)); // Reset the LFN entry array to empty
                longfile_count = 0;                                 // Reset the LFN counter to 0
            }
            if (longfile_count > 0)
            {

                char name[9];
                char ext[4];
                short_filename(entry, name, ext);

                char lname[256] = "";
                long_filename(long_entry, longfile_count, lname);
                longfile_count = 0;
                memset(long_entry, 0, 11 * sizeof(fat32_Longfile));

                // Extract the date and time
                int year, month, day, hour, minute, am_pm;
                Extract_Date_Time(entry, &year, &month, &day, &hour, &minute, &am_pm);

                // Combine the high and low cluster numbers to get the actual cluster number
                unsigned int cluster = (entry.sfile.first_cluster_high << 16) | entry.sfile.first_cluster_low;

                // Get the file size and format it with commas
                int tempFileSize = entry.sfile.size;
                int lengthOfFileSize = 0;
                while (tempFileSize != 0)
                {
                    tempFileSize /= 10;
                    lengthOfFileSize++;
                }
                char *fileSize = (char *)malloc(sizeof(char) * lengthOfFileSize);
                sprintf(fileSize, "%u", entry.sfile.size);
                // addCommas(fileSize);

                if ((entry.sfile.attributes & 0x10) == 0x10) // Check if entry is a directory
                {

                    printf("%02d/%02d/%04d %02d:%02d %2s %-10s %8u",
                           month, day, year,
                           hour, minute, am_pm == 1 ? "PM" : "AM",
                           "<DIR>", cluster);
                    printf("%-22s", "");
                    int counter = 0;
                    for (int i = 0; i < 9; i++)
                    {
                        if (name[i] != ' ' && name[i] != '\0')
                        {
                            counter++;
                            printf("%c", name[i]);
                        }
                        else
                            continue;
                    }
                    counter = 15 - counter;
                    printf("%-*s ", counter, "");
                    printf("%s\n", lname);
                    dircount++;
                }
                else
                {

                    printf("%02d/%02d/%04d %02d:%02d %2s %-10s %8u %12s", month, day, year, hour, minute, am_pm == 1 ? "PM" : "AM", "", cluster, fileSize);
                    // Print File Name
                    int counter = 0;
                    printf("%-9s", "");
                    for (int i = 0; i < 9; i++)
                    {
                        if (name[i] != ' ' && name[i] != '\0')
                        {
                            counter++;
                            printf("%c", name[i]);
                        }
                        else
                            break;
                    }
                    printf(".");
                    for (int i = 0; i < 4; i++)
                    {
                        if (ext[i] != ' ' && ext[i] != '\0')
                        {
                            counter++;
                            printf("%c", ext[i]);
                        }
                        else
                            break;
                    }
                    counter = 15 - counter;
                    printf("%-*s", counter, "");
                    printf("%s\n", lname);

                    fcounter++;
                    bytes_size += entry.sfile.size;
                }
            }
            else
            {
                char filename[9];
                char extension[4];
                short_filename(entry, filename, extension);

                // Extract date and time information
                int year, month, day, hour, minute, am_pm;
                Extract_Date_Time(entry, &year, &month, &day, &hour, &minute, &am_pm);

                // Get actual cluster number
                unsigned int cluster = (entry.sfile.first_cluster_high << 16) | entry.sfile.first_cluster_low;

                // Get file size and format it with commas
                int temp_size = entry.sfile.size;
                int filesize_length = 0;
                while (temp_size != 0)
                {
                    temp_size /= 10;
                    filesize_length++;
                }
                char *filesize_str = (char *)malloc(sizeof(char) * filesize_length);
                sprintf(filesize_str, "%u", entry.sfile.size);
                // addCommas(filesize_str);

                if ((entry.sfile.attributes & 0x10) == 0x10)
                {
                    printf("%02d/%02d/%04d %02d:%02d %2s %-10s %8u", month, day, year,
                           hour, minute, am_pm == 1 ? "PM" : "AM", "<DIR>", cluster);
                    printf("%-38s", "");
                    for (int i = 0; i < 9; i++)
                    {
                        if (filename[i] != ' ' && filename[i] != '\0')
                            printf("%c", filename[i]);
                        else
                            continue;
                    }
                    printf("\n");
                    dircount++;
                }
                else
                {
                    printf("%02d/%02d/%04d %02d:%02d %2s %-10s %8u %12s", month, day, year,
                           hour, minute, am_pm == 1 ? "PM" : "AM", "", cluster, filesize_str);
                    printf("%-24s ", "");
                    for (int i = 0; i < 9; i++)
                    {
                        if (filename[i] != ' ' && filename[i] != '\0')
                        {
                            printf("%c", filename[i]);
                        }

                        else
                        {
                            continue;
                        }
                    }
                    printf(".");
                    for (int i = 0; i < 4; i++)
                    {
                        if (extension[i] != ' ' && extension[i] != '\0')
                            printf("%c", extension[i]);
                        else
                            continue;
                    }
                    printf("\n");
                    fcounter++;
                    bytes_size += entry.sfile.size; // Add the file size to the total file bytes count
                }
            }
        }
    }

    int totalsize = 0;
    int total_size = bytes_size;
    while (total_size > 0)
    {
        total_size /= 10;
        totalsize++;
    }
    if (total_size == NULL)
    {
        printf("Error: unable to allocate memory\n");
        return 1;
    }
    char *filesize = malloc(sizeof(char) * totalsize);

    printf("%15d File(s)\t%10s bytes\n%15d Dir(s)\n", fcounter, filesize, dircount);
    
}
