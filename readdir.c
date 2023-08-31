#include "helper.h"

int main()
{
    FILE *fp;
    char filename[] = "Directory.dat";
    struct fat32_file dir_entry;
    int i = sizeof(dir_entry);
    int num_entries = 0;

    // open file for reading
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("File not found.\n");
        return 1;
    }

    // read file till end of file and store in array of structs
    while (fread(&dir_entry, sizeof(struct fat32_file), 1, fp) == 1)
    {

        num_entries++;

        // check if attribute is 0x0f
        if (islongfile(dir_entry))
        {

            continue;
        }
    }

    fclose(fp);

    printf("Number of directory entries: %d\n", num_entries);

    // open file for reading again
    fp = fopen(filename, "rb");
    printf("Date        time          size      filename \n");

    // read and print all directory entries
    for (int i = 0; i < num_entries; i++)
    {
        // unsigned int day, month, year;

        fread(&dir_entry, sizeof(struct fat32_file), 1, fp);
        
        // decode_write_date(dir_entry.write_date, &day, &month, &year);
        int day, month, year;
        decode_write_date(dir_entry.write_date, &day, &month, &year);
        printf("%u/%u/%u", month, day, year);

        int hours, minutes, seconds;
        decode_write_time(dir_entry.creation_time, &hours, &minutes, &seconds);
        printf("     %u:%02u %cM", (unsigned)hours, (unsigned)minutes, (hours >= 12 ? 'P' : 'A'));

        if (dir_entry.attributes & ATTR_DIRECTORY)
        {
            printf("  <Dir>  ");
        }
        if (dir_entry.attributes & ATTR_DIRECTORY)
        {
            printf("<Dir>");
        }
        else
        {
            printf("     ");
        }

        // unsigned short hi = (dir_entry[i].first_cluster_high[1] << 8) | dir_entry[i].first_cluster_high[0];
        // unsigned short lo = (dir_entry[i].first_cluster_low[1] << 8) | dir_entry[i].first_cluster_low[0];
        unsigned int cluster_number = (dir_entry.first_cluster_high << 16) | dir_entry.first_cluster_low;

        printf("    %d", cluster_number);
        printf("     %u", dir_entry.size);

        format_filename(dir_entry.filename);
        printf("    %s\n", dir_entry.filename);
    }
    fclose(fp);

    return 0;
}
