#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
#include<string.h>

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;

    // Fill with sample filenames
    // encInfo.src_image_fname = "beautiful.bmp";
    // encInfo.secret_fname = "secret.txt";
    // encInfo.stego_image_fname = "stego_img.bmp";


    { //Checking for Operation type
        if(argc>=3)
        {
            if(check_operation_type(argv)==e_encode && (argc >3 && argc <6))
                printf("INFO : User Selected Encode Operation\n");

            else if (check_operation_type(argv)==e_decode && (argc >2 && argc <5))
                printf("INFO : User Selected Decode Operation\n");

            else if(check_operation_type(argv) == e_unsupported)
                printf("\nError: Unsupported Operation\n");

            else
            {
                printf("\nError: Insufficient Arguments for Selected Operation\n");
                return 1;
            }
        }
        else
        {
            printf("\nError: Insufficient Arguments\n"
                    "For encoding : ./a.out -e <input_img.bmp> <secret_msg_file> <output_img.bmp>\n"
                    "For decoding : ./a.out -d <input_img.bmp> <output_sec_file>\n");
            return 1;
        }
        
    }

    //Encoding Data
    if(check_operation_type(argv)==e_encode)
    {
        {   //Validating arguments
            if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success);
            else
                return 1;
        }


        //Test open_files
        printf("INFO: Opening required files\n");
        if (open_files(&encInfo) == e_failure)
        {
            printf("\nError: %s function failed\n", "open_files" );
            return 1;
        }
        else
            printf("INFO: Done\n");



        // Test get_image_size_for_bmp
        // img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
        // printf("INFO: Image size = %u\n", img_size);

        
        printf("INFO: ## Encoding Procedure Started ##\n");
        {   //Checking Capacity of src file
            if(check_capacity(&encInfo) == e_success)
                printf("INFO: Done. Found OK\n");
            else
            {
                printf("\nImage size is not sufficient\n");
                return 1;
            }
        }

        {   //Copying header data
            printf("INFO: Copying Image Header\n");
            if(copy_bmp_header(encInfo.fptr_src_image, encInfo.fptr_stego_image)==e_success)
                printf("INFO: Done\n");
            else
            {
                printf("\nError: Copying Header Data of bmp is Failed\n");
                return 1;
            }

        }

        {   //Encoding Data
            if(do_encoding(&encInfo) == e_success)
                printf("INFO: ## Encoding Done Successfully ##\n");
            else
            {
                printf("\nError : Encoding Failed\n");
                return 1;
            }
        }

    }
    else if(check_operation_type(argv) == e_encode)
        printf("\nError : Too Many Arguments\n");

   
   
   
   
   
   
   
   
   
    //Decoding data
    if (check_operation_type(argv)==e_decode && argc <5)
    {
        printf("INFO: ## Decoding Procedure Started ##\n");
        decInfo.size_sec_file_ext = 0;
        decInfo.size_secret_file = 0;

        decInfo.argc = argc;

        {
            if(read_and_validate_decode_args(argc, argv, &decInfo) == e_success);
            else
                return 1;
        }


        {
            printf("INFO: Opening required files\n");
            if (open_files_dec(&decInfo) == e_failure)
            {
                printf("\nERROR: %s function failed\n", "open_files" );
                return 1;
            }
        }


        {   //Decoding magic string
            char magic_string[10];

            printf("Enter the Magic String : ");
            scanf("%[^\n]",magic_string);

            printf("INFO: Decoding Magic String Signature\n");
            if(decode_magic_string(magic_string, &decInfo) == e_success)
                printf("INFO: _Access Granted_\n");
            else
            {
                printf("\nError : Enter a Valid Magic String Signature\n");
                return 1;
            }
        }

        {   //Decoding file extension size
            if(do_decoding(&decInfo) == e_success)
                printf("INFO: ## Decoding Done Successfully ##\n");
            else
            {
                printf("\nError : Decoding Failed\n");
                return 1;
            }
        }

    }
    else if(check_operation_type(argv) == e_decode)
        printf("\nError : Too Many Arguments\n");



    return 0;
}
