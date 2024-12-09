#include <string.h>
#include <getopt.h>

#include "flags/flags.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* flags_strerror(const enum FlagsError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(FLAGS_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FLAGS_ERROR_FAILURE);
        default:
            return "UNKNOWN_FLAGS_ERROR";
    }
    return "UNKNOWN_FLAGS_ERROR";
}
#undef CASE_ENUM_TO_STRING_


enum FlagsError flags_objs_ctor(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    if (!strncpy(flags_objs->log_folder, "./log/", FILENAME_MAX_SIZE))
    {
        perror("Can't strncpy flags_objs->log_folder");
        return FLAGS_ERROR_SUCCESS;
    }

    if (!strncpy(flags_objs->in_filename, "./assets/input.txt", FILENAME_MAX_SIZE))
    {
        perror("Can't strncpy flags_objs->log_folder");
        return FLAGS_ERROR_SUCCESS;
    }

    if (!strncpy(flags_objs->out_filename, "./assets/output.tex", FILENAME_MAX_SIZE))
    {
        perror("Can't strncpy flags_objs->log_folder");
        return FLAGS_ERROR_SUCCESS;
    }

    flags_objs->in_file  = NULL;
    flags_objs->out_file = NULL;

    flags_objs->mode = MODE_NDIFF;

    return FLAGS_ERROR_SUCCESS;
}

enum FlagsError generate_pdf_(flags_objs_t* const flags_objs);

enum FlagsError flags_objs_dtor (flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    if (flags_objs->in_file && fclose(flags_objs->in_file))
    {
        perror("Can't fclose in_file");
        return FLAGS_ERROR_FAILURE;
    }
    IF_DEBUG(flags_objs->in_file  = NULL;)

    if (flags_objs->out_file)
    {
        fprintf(flags_objs->out_file, "\n\\end{document}");

        if (fclose(flags_objs->out_file))
        {
            perror("Can't fclose out_file");
            return FLAGS_ERROR_FAILURE;
        }
    }
    IF_DEBUG(flags_objs->out_file = NULL;)

    FLAGS_ERROR_HANDLE(generate_pdf_(flags_objs));

    return FLAGS_ERROR_SUCCESS;
}

enum FlagsError generate_pdf_(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    char pdf_directory[FILENAME_MAX_SIZE] = {};

    if (!strncpy(pdf_directory, flags_objs->out_filename, FILENAME_MAX_SIZE))
    {
        perror("Can't strncpy pdf_directory");
        return FLAGS_ERROR_FAILURE;
    }

    *strchr(pdf_directory, '.') = '\0';

    char system_cmd[FILENAME_MAX_SIZE] = {};

    if (snprintf(system_cmd, FILENAME_MAX_SIZE, "mkdir -p %s", pdf_directory) < 0)
    {
        perror("Can't snprintf system cmd mkdir");
        return FLAGS_ERROR_FAILURE;
    }

    if (system(system_cmd) == -1)
    {
        perror("Can't system mkdir pdf");
        return FLAGS_ERROR_FAILURE;
    }

    if (snprintf(system_cmd, FILENAME_MAX_SIZE, "pdflatex --output-directory=%s %s", //FIXME
                 pdf_directory, flags_objs->out_filename) < 0)
    {
        perror("Can't snprintf system cmd");
        return FLAGS_ERROR_SUCCESS;
    }

    if (system(system_cmd) == -1)
    {
        perror("Can't system create pdf");
        return FLAGS_ERROR_FAILURE;
    }

    return FLAGS_ERROR_SUCCESS;
}

enum FlagsError flags_processing(flags_objs_t* const flags_objs, 
                                 const int argc, char* const argv[])
{
    lassert(!is_invalid_ptr(flags_objs), "");
    lassert(!is_invalid_ptr(argv), "");
    lassert(argc, "");

    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "l:o:i:m:")) != -1)
    {
        switch (getopt_rez)
        {
            case 'l':
            {
                if (!strncpy(flags_objs->log_folder, optarg, FILENAME_MAX_SIZE))
                {
                    perror("Can't strncpy flags_objs->log_folder");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 'o':
            {
                if (!strncpy(flags_objs->out_filename, optarg, FILENAME_MAX_SIZE))
                {
                    perror("Can't strncpy flags_objs->out_filename");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 'i':
            {
                if (!strncpy(flags_objs->in_filename, optarg, FILENAME_MAX_SIZE))
                {
                    perror("Can't strncpy flags_objs->in_filename");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 'm':
            {
                if (strcmp(optarg, "ndiff") == 0)
                {
                    flags_objs->mode = MODE_NDIFF;
                }
                else if (strcmp(optarg, "taylor") == 0)
                {
                    flags_objs->mode = MODE_TAILOR;
                }
                else if (strcmp(optarg, "test") == 0)
                {
                    flags_objs->mode = MODE_TEST;
                }
                else
                {
                    fprintf(stderr, "Unknown mode\n");
                    return FLAGS_ERROR_FAILURE;
                }
                break;
            }

            default:
            {
                fprintf(stderr, "Getopt error - d: %d, c: %c\n", getopt_rez, (char)getopt_rez);
                return FLAGS_ERROR_FAILURE;
            }
        }
    }

    if (strcmp(flags_objs->in_filename, flags_objs->out_filename) == 0)
    {
        fprintf(stderr, "Equal inout and output filename\n");
        return FLAGS_ERROR_FAILURE;
    }

    if (!(flags_objs->in_file = fopen(flags_objs->in_filename, "rb")))
    {
        perror("Can't fopen in_file");
        return FLAGS_ERROR_FAILURE;
    }

    if (!(flags_objs->out_file = fopen(flags_objs->out_filename, "wb")))
    {
        perror("Can't fopen out_file");
        return FLAGS_ERROR_FAILURE;
    }

    fprintf(flags_objs->out_file, 
        "\\documentclass[a4paper]{article}\n"
        "\\usepackage{amsmath}\n"
        "\\usepackage[T2A]{fontenc}\n"
        "\\usepackage[english, russian]{babel}\n"
        "\\usepackage{mathtools}\n"
        "\\usepackage{amsfonts}\n"              
        "\\usepackage{graphicx}\n"  
        "\\usepackage{wrapfig}\n"
        "\n"
        "\\title{Что-то дифферинцируется, а что-то нет, но это не плохо)}\n"
        "\\author{Владимир Швабра}\n"
        "\\date{\\today}"            
        "\n"
        "\\begin{document}\n"
        "\\maketitle\n"
        "\\newpage\n"
        "\n");

    return FLAGS_ERROR_SUCCESS;
}