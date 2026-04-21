#include "core.h"

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        char * filename = argv[i];
        
        printf("Accessing '%s'\n", filename);
        int is_exists = Exists(filename);
        if (is_exists == 0)
        {
            printf("Unable to access '%s'\n", filename);
            return 0;
        }
        
        printf("Overwriting '%s'\n", filename);
        int is_overwrite = Overwrite(filename);
        if (is_overwrite == -1)
        {
            printf("Unable to overwrite '%s'\n", filename);
            return 0;
        }

        printf("Verifying '%s'\n", filename);
        int is_valid = Verify(filename);
        if (is_valid == 0)
        {
            printf("%s\n", "Failed");
        }
        else if (is_valid == 1)
        {
            printf("%s\n", "Success");
        }
        else
        {
            printf("Unable to verify '%s'\n", filename);
            return 0;
        }
    }
    return 0;
}