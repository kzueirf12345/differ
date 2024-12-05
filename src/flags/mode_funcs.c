#include "flags/mode_funcs.h"
#include "flags/flags.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree/funcs/funcs.h"

int mode_ndiff (flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    return FLAGS_ERROR_SUCCESS;
}
int mode_taylor(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    return FLAGS_ERROR_SUCCESS;
}
int mode_test  (flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    tree_t* tree2 = NULL;
    TREE_ERROR_HANDLE(tree_read(flags_objs->in_filename, &tree2));

    TREE_ERROR_HANDLE(tree_simplify(&tree2, stdout),
                                                                                   tree_dtor(tree2);
    );

    tree_t* tree2_val = tree_val_in_point(tree2, 'x', 0.5, stdout);
    if (tree2_val == NULL)
    {
        fprintf(stderr, "Can't tree_val_in_point\n");
                                                                                   tree_dtor(tree2);
        return EXIT_FAILURE;
    }

    tree_t* tree2_diff = tree_diff(tree2, NULL, 'x', stdout);

    tree_t* tree2_ndiff = tree_ndiff(tree2, 1, 'x', stdout);

    tree_t* tree2_monom = tree_taylor_polynom(tree2, 10, 'x', 0.1, stdout);

    TREE_ERROR_HANDLE(tree_create_graphic(tree2_monom, 'x', "assets/polynom_sin", "sin", 1000),
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_simplify(&tree2_diff, stdout),
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );


    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree2),          
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree2_val),
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree2_diff),          
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree2_ndiff),          
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree2_monom),          
                 tree_dtor(tree2_diff);tree_dtor(tree2);tree_dtor(tree2_val);tree_dtor(tree2_ndiff);
                                                                             tree_dtor(tree2_monom);
    );


    tree_dtor(tree2);
    tree_dtor(tree2_diff);
    tree_dtor(tree2_val);
    tree_dtor(tree2_ndiff);
    tree_dtor(tree2_monom);


    return FLAGS_ERROR_SUCCESS;
}