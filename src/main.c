#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

#include "flags/flags.h"
#include "logger/liblogger.h"
#include "tree/funcs/funcs.h"
#include "tree/verification/dumb.h"

int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
int dtor_all(flags_objs_t* const flags_objs);

int main(const int argc, char* const argv[])
{
    flags_objs_t flags_objs = {};

    if (init_all(&flags_objs, argc, argv))
    {
        fprintf(stderr, "Can't init all\n");
        return EXIT_FAILURE;
    }

    tree_t* tree1 = 
        tree_ctor((tree_data_u){.op = OP_TYPE_DIV}, NODE_TYPE_OP, NULL,
            tree_ctor((tree_data_u){.op = OP_TYPE_SUB}, NODE_TYPE_OP, NULL,
                tree_ctor((tree_data_u){.num = 1 }, NODE_TYPE_VAR, NULL, NULL, NULL),
                tree_ctor((tree_data_u){.num = 3 }, NODE_TYPE_NUM, NULL, NULL, NULL)),
            tree_ctor((tree_data_u){.op = OP_TYPE_SUM}, NODE_TYPE_OP, NULL,
                tree_ctor((tree_data_u){.num = 1000 }, NODE_TYPE_NUM, NULL, NULL, NULL),
                tree_ctor((tree_data_u){.num = 7 },    NODE_TYPE_NUM, NULL, NULL, NULL)));

    tree_fill_pt(tree1, NULL);

    tree_t* tree1_diff = tree_diff(tree1, NULL, 'x', stdout);

    tree_t* tree2 = NULL;
    TREE_ERROR_HANDLE(tree_read(flags_objs.in_filename, &tree2),
                                       tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
    );

    TREE_ERROR_HANDLE(tree_simplify(&tree2, stdout),
                      tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
    );

    tree_t* tree2_val = tree_val_in_point(tree2, 'x', 0.5, stdout);
    if (tree2_val == NULL)
    {
        fprintf(stderr, "Can't tree_val_in_point\n");
                      tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
        return EXIT_FAILURE;
    }

    tree_t* tree2_diff = tree_diff(tree2, NULL, 'x', stdout);

    tree_t* tree2_ndiff = tree_ndiff(tree2, 1, 'x', stdout);

    tree_t* tree2_monom = tree_taylor_polynom(tree2, 10, 'x', 0.1, stdout);

    TREE_ERROR_HANDLE(tree_create_graphic(tree2_monom, 'x', "assets/polynom_sin", "sin", 1000),
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_simplify(&tree2_diff, stdout),
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );


    TREE_ERROR_HANDLE(tree_print_tex(flags_objs.out_file, tree2),          
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs.out_file, tree2_val),
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs.out_file, tree2_diff),          
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs.out_file, tree2_ndiff),          
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs.out_file, tree2_monom),          
tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree1_diff);tree_dtor(tree1);dtor_all(&flags_objs);
tree_dtor(tree2_val);tree_dtor(tree2_ndiff);tree_dtor(tree2_monom);
    );

    tree_dtor(tree1);
    tree_dtor(tree1_diff);
    tree_dtor(tree2);
    tree_dtor(tree2_diff);
    tree_dtor(tree2_val);
    tree_dtor(tree2_ndiff);
    tree_dtor(tree2_monom);

    
    if (dtor_all(&flags_objs))
    {
        fprintf(stderr, "Can't dtor all\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

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
}
#undef LOGOUT_FILENAME
#undef   DUMB_FILENAME