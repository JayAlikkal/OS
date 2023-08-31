#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char filename[8];             // 11 characters for the filename
    char ext[3];                  // 3 char extension
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
} fat32_shortfile;

typedef struct
{
    unsigned char Order;              // sequence number of the LFN entry
    unsigned char fname1[10];         // characters 1-5 of the long-name sub-component
    unsigned char attributes;         // attribute byte (always 0x0F)
    unsigned char Type;               // type (always 0)
    unsigned char Checksum;           // checksum of the short file name entry
    unsigned char fname2[12];         // characters 6-11 of the long-name sub-component
    unsigned short first_cluster_low; // low 16 bits of the first cluster number should be 0
    unsigned char fname3[4];          // characters 12-13 of the long-name sub-component
} fat32_Longfile;

typedef union
{
    fat32_shortfile sfile;
    fat32_Longfile lfile;
} filetype;

void short_filename(filetype entry, char filename[9], char ext[4])
{
    strncpy(filename, entry.sfile.filename, 8);
    filename[8] = '\0';
    strncpy(ext, entry.sfile.ext, 3);
    ext[3] = '\0';
}

void long_filename(fat32_Longfile *long_entry, int longfile_count, char *lname)
{
    for (int i = longfile_count; i >= 0; i--)
    {
       
        for (int j = 0; j < 10; j += 2)
        {
            
            if (long_entry[i].fname1[j] == 0x00 || long_entry[i].fname1[j] == 255)
            {
                
                break;
            }
            char c[2] = {long_entry[i].fname1[j], '\0'};  

            
            if (long_entry[i].fname2[j] == 0x00 || long_entry[i].fname2[j] == 255)
            {
                
                break;
            }
            c[0] = long_entry[i].fname2[j];

           
            if (long_entry[i].fname3[j] == 0x00 || long_entry[i].fname3[j] == 255)
            {
               
                break;
            }
            c[0] = long_entry[i].fname3[j]; 
            strcat(lname, c);  
        }        
    }
}

void Extract_Date_Time(filetype entry, int *year, int *month, int *day, int *hour, int *minute, int *am_pm)
{
    // Extract day value (bits 0-4)
    *day = entry.sfile.write_date & 0x1F;

    // Extract month value (bits 5-8)
    *month = (entry.sfile.write_date >> 5) & 0x0F;

    // Extract year value (bits 9-15)
    *year = ((entry.sfile.write_date >> 9) & 0x7F) + 1980;

    // Extract the high byte(bits 8 - 15) and the low byte(bits 0 - 7)
    uint8_t high_byte = (entry.sfile.write_time >> 8) & 0xFF;
    uint8_t low_byte = entry.sfile.write_time & 0xFF;

    // Extract seconds (bits 0-4)
    //*seconds = (swapped_write_time & 0x1F) * 2;

    // Extract minutes (bits 5-10)
    *minute = (entry.sfile.write_time >> 5) & 0x3F;

    // Extract hours (bits 11-15)
    *hour = (entry.sfile.write_time >> 11) & 0x1F;

    *am_pm = *hour >= 12 ? 1 : 0;
    *hour = *hour % 12;
    if (*hour == 0)
    {
        *hour = 12;
    }
}
