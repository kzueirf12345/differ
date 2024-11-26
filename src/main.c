#include <stdio.h>
#include <locale.h>

#include "logger/liblogger.h"
#include "flags/flags.h"
#include "tree/funcs.h"

// int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
// int dtor_all(flags_objs_t* const flags_objs);

const char* s = "25*10*(3*(25-10*2)+1)$";
size_t      p = 0;

int GetG();
int GetE();
int GetT();
int GetP();
int GetN();

#define SyntaxError \
    fprintf(stderr, "syntax error line: %d\n", __LINE__); \
    exit(0)


int main(const int argc, char* const argv[])
{

/*
    flags_objs_t flags_objs = {};

    if (init_all(&flags_objs, argc, argv))
    {
        fprintf(stderr, "Can't init all\n");
        return EXIT_FAILURE;
    }

    tree_t* const div  = tree_ctor((tree_data_u){.op = OP_TYPE_DIV}, NODE_TYPE_OP, NULL, NULL, NULL);

    tree_t* const sub  = tree_ctor((tree_data_u){.op = OP_TYPE_SUB}, NODE_TYPE_OP, div, NULL, NULL);
    tree_t* const mul  = tree_ctor((tree_data_u){.op = OP_TYPE_SUM}, NODE_TYPE_OP, div, NULL, NULL);


    tree_t* const x    = tree_ctor((tree_data_u){.num = 1 },    NODE_TYPE_VAR, mul, NULL, NULL);
    tree_t* const num1 = tree_ctor((tree_data_u){.num = 3 },    NODE_TYPE_NUM, mul, NULL, NULL);
    tree_t* const num2 = tree_ctor((tree_data_u){.num = 1000 }, NODE_TYPE_NUM, sub, NULL, NULL);
    tree_t* const num3 = tree_ctor((tree_data_u){.num = 7 },    NODE_TYPE_NUM, sub, NULL, NULL);
    
    mul->lt = x;
    mul->rt = num1;
    mul->size += 2;

    sub->lt = num2;
    sub->rt = num3;
    sub->size += 2;

    div->lt = mul;
    div->rt = sub;
    div->size += mul->size + sub->size;


    // FILE* file = fopen("./assets/temp.tex", "wb");
    // if (!file)
    // {
    //     perror("Can't fopen file");
    //     tree_dtor(div), dtor_all(&flags_objs);
    //     return EXIT_FAILURE;
    // }

    // tree_t* const div_diff = tree_diff(div, file);
    // tree_dumb(div_diff);
    // TREE_ERROR_HANDLE(tree_print_tex(file, div_diff),
    //                                      tree_dtor(div_diff), tree_dtor(div), dtor_all(&flags_objs);
    // );

    // if (fclose(file))
    // {
    //     perror("Can't fclose file");
    //     tree_dtor(div_diff), tree_dtor(div), dtor_all(&flags_objs);
    //     return EXIT_FAILURE;
    // }

    // if (system("pdflatex --output-directory=./assets/ assets/temp.tex") == -1)
    // {
    //     perror("Can't system create pdf");
    //     tree_dtor(div_diff), tree_dtor(div), dtor_all(&flags_objs);
    //     return EXIT_FAILURE;
    // }

    // tree_dtor(div_diff);


    // tree_dumb(div);

    // FILE* file = fopen("./assets/baze.txt", "wb");
    // if (!file)
    // {
    //     perror("Can't fopen file");
    //     return EXIT_FAILURE;
    // }
    // TREE_ERROR_HANDLE(tree_print_preorder(file, div),     
    //                                                           tree_dtor(div), dtor_all(&flags_objs);
    // );

    // printf("\n");
    // TREE_ERROR_HANDLE(tree_print_inorder(stdout, div),        tree_dtor(div), dtor_all(&flags_objs);
    // );

    // if (fclose(file))
    // {
    //     perror("Can't fclose file");
    //     return EXIT_FAILURE;
    // }
    // file = NULL;

    tree_t* temp = NULL;
    TREE_ERROR_HANDLE(tree_read_inorder("./assets/temp2.txt", &temp),
                                             tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
    );


    tree_t* new_temp = tree_diff(temp, NULL, stdout);
    if (!new_temp)
    {
        fprintf(stderr, "Can't tree copy temp\n");
        tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
        return EXIT_FAILURE;
    }

    tree_dumb(new_temp);

    FILE* file = fopen("./assets/temp2.tex", "wb");
    if (!file)
    {
        perror("Can't fopen file");
        tree_dtor(new_temp); tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
        return EXIT_FAILURE;
    }
    TREE_ERROR_HANDLE(tree_print_tex(file, new_temp),     
                        tree_dtor(new_temp); tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
    );
    if (fclose(file))
    {
        perror("Can't fclose file");
        tree_dtor(new_temp); tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
        return EXIT_FAILURE;
    }

    if (system("pdflatex --output-directory=./assets/ assets/temp2.tex") == -1)
    {
        perror("Can't system create pdf");
        tree_dtor(new_temp); tree_dtor(temp), tree_dtor(div), dtor_all(&flags_objs);
        return EXIT_FAILURE;
    }



    tree_dtor(div);
    tree_dtor(temp);
    tree_dtor(new_temp);

    if (dtor_all(&flags_objs))
    {
        fprintf(stderr, "Can't dtor all\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//==================================================================================================

int logger_init(char* const log_folder);

int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv)
{
    lassert(argc, "");
    lassert(argv, "");

    if (!setlocale(LC_ALL, "ru_RU.utf8"))
    {
        fprintf(stderr, "Can't setlocale\n");
        return EXIT_FAILURE;
    }

    FLAGS_ERROR_HANDLE(flags_objs_ctor (flags_objs));
    FLAGS_ERROR_HANDLE(flags_processing(flags_objs, argc, argv));

    if (logger_init(flags_objs->log_folder))
    {
        fprintf(stderr, "Can't logger init\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int dtor_all(flags_objs_t* const flags_objs)
{
    LOGG_ERROR_HANDLE(                                                               logger_dtor());
    TREE_DUMB_ERROR_HANDLE(                                                       tree_dumb_dtor());
    FLAGS_ERROR_HANDLE(                                                flags_objs_dtor(flags_objs));
    return EXIT_SUCCESS;
}

#define LOGOUT_FILENAME "logout.log"
#define   DUMB_FILENAME "dumb"
int logger_init(char* const log_folder)
{
    lassert(log_folder, "");

    char logout_filename[FILENAME_MAX_SIZE] = {};
    if (snprintf(logout_filename, FILENAME_MAX_SIZE, "%s%s", log_folder, LOGOUT_FILENAME) <= 0)
    {
        perror("Can't snprintf logout_filename");
        return EXIT_FAILURE;
    }

    char dumb_filename[FILENAME_MAX_SIZE] = {};
    if (snprintf(dumb_filename, FILENAME_MAX_SIZE, "%s%s", log_folder, DUMB_FILENAME) <= 0)
    {
        perror("Can't snprintf dumb_filename");
        return EXIT_FAILURE;
    }


    LOGG_ERROR_HANDLE(logger_ctor());
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL));
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename));

    TREE_DUMB_ERROR_HANDLE(tree_dumb_ctor());
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_file(dumb_filename));
    
    return EXIT_SUCCESS;
*/

    printf("ans: %d\n", GetG());

    return EXIT_SUCCESS;
}
#undef LOGOUT_FILENAME
#undef   DUMB_FILENAME

int GetG()
{
    int val = GetE();
    if (s[p] != '$')
    {
        SyntaxError;
    }
    return val;
}

int GetE()
{
    int val = GetT();
    while(s[p] == '+' || s[p] == '-')
    {
        int op = s[p];
        ++p;
        int val2 = GetT();
        if (op == '+') val += val2;
        else           val -= val2;
    }
    return val;
}

int GetT()
{
    int val = GetP();
    while(s[p] == '*' || s[p] == '/')
    {
        int op = s[p];
        ++p;
        int val2 = GetP();

        if (op == '*') val *= val2;
        else           val /= val2;
    }
    return val;
}

int GetP()
{
    if (s[p] == '(')
    {
        ++p;
        int val = GetE();
        if (s[p] != ')')
        {
            SyntaxError;
        }
        ++p;
        return val;
    }
    
    return GetN();
}

int GetN()
{
    size_t old_p = p;
    int val = 0;
    while ('0' <= s[p] && s[p] <= '9')
    {
        val = val * 10 + s[p] - '0';
        ++p;
    }
    if (p == old_p)
    {
        SyntaxError;
    }
    return val;
}