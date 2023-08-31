#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

struct fat32_file
{
    char filename[11];            // 11 characters for the filename
    uint8_t attributes;           // File attributes
    uint8_t ntres;                // Reserved for use by Windows NT
    uint8_t creation_time_tenths; // Millisecond stamp at file creation time
    uint16_t creation_time;       // Time file was created
    uint16_t creation_date;       // Date file was created
    uint16_t last_access_date;    // Last access date
    uint16_t first_cluster_high;  // High word of this entry's first cluster number (always 0 for FAT12/FAT16)
    uint16_t write_time;          // Time of last write
    uint16_t write_date;          // Date of last write
    uint16_t first_cluster_low;   // Low word of this entry's first cluster number
    uint32_t size;                // File size in bytes
};

bool islongfile(struct fat32_file file)
{
    return file.attributes == 0x0f;
}

void decode_write_date(uint16_t write_date, int *day, int *month, int *year)
{

    // Extract day value (bits 0-4)
    *day = write_date & 0x1F;

    // Extract month value (bits 5-8)
    *month = (write_date >> 5) & 0x0F;

    // Extract year value (bits 9-15)
    *year = ((write_date >> 9) & 0x7F) + 1980;
}

void decode_write_time(uint16_t write_time, int *hours, int *minutes, int *seconds)
{
    // Extract the high byte (bits 8-15) and the low byte (bits 0-7)
    uint8_t high_byte = (write_time >> 8) & 0xFF;
    uint8_t low_byte = write_time & 0xFF;

    // Swap the high byte and the low byte
    uint16_t swapped_write_time = (low_byte << 8) | high_byte;

    // Extract seconds (bits 0-4)
    *seconds = (swapped_write_time & 0x1F) * 2;

    // Extract minutes (bits 5-10)
    *minutes = (swapped_write_time >> 5) & 0x3F;

    // Extract hours (bits 11-15)
    *hours = (swapped_write_time >> 11) & 0x1F;

    // Convert to 12-hour format and add AM/PM suffix
    char am_pm = 'A';
    if (*hours >= 12)
    {
        am_pm = 'P';
        if (*hours > 12)
        {
            *hours -= 12;
        }
    }
    if (*hours == 0)
    {
        *hours = 12;
    }
}

void remove_trailing_spaces(char *filename)
{
    int len = strlen(filename);
    while (len > 0 && isspace(filename[len - 1]))
    {
        len--;
    }
    filename[len] = '\0';
}

void format_filename(char *filename)
{

    remove_trailing_spaces(filename);
    int len = strlen(filename);
    int i = 0, j = 0;
    for (i = 0; i < 8 && i < len && !isspace(filename[i]); i++)
    {
        // Copy the first 8 non-space characters to the filename buffer
        filename[i] = toupper(filename[i]);
        //
    }
    filename[i++] = '.';
    for (int j = 0; j < 3 && i + j < len && !isspace(filename[i + j]); j++)
    {
        // Copy the last 3 non-space characters (extension) to the filename buffer
        filename[i + j] = toupper(filename[i + j]);
    }
    filename[i + j] = '\0';
}