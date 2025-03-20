#include <stdio.h>
#include <string.h>

#include "mode_funcs.h"
#include "flags/flags.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree/funcs/funcs.h"

int mode_ndiff (flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    printf("Вы находитесь в режиме вычисления n-ой производной\n");

    char diff_var = 'x';
    do
    {
        printf("Введите переменную дифферинцирования:\n");
        diff_var = (char)getchar();
    } 
    while (diff_var < 'a' || 'z' < diff_var || diff_var == 'e');

    size_t diff_order = 0;
    do
    {
        scanf("%*[^\n]");
        printf("Введите порядок дифферинцирования:\n");
    } 
    while (scanf("%zu", &diff_order) <= 0);

    tree_t* source = NULL;
    TREE_ERROR_HANDLE(tree_read(flags_objs->in_filename, &source));

    fprintf(flags_objs->out_file, "\\subsection{Исходная функция:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, source), 
                                                                                  tree_dtor(source);
    );

    TREE_ERROR_HANDLE(tree_simplify(&source, flags_objs->out_file),           
                                                                                  tree_dtor(source);
    );

    fprintf(flags_objs->out_file, "\\subsection{Упростим её:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, source), 
                                                                                  tree_dtor(source);
    );

    tree_t* tree_deriv = tree_ndiff(source, diff_order, diff_var, flags_objs->out_file);

    if (!tree_deriv)
    {
        fprintf(stderr, "Can't tree_diff\n");
                                                                                  tree_dtor(source);
        return EXIT_FAILURE;
    }

    fprintf(flags_objs->out_file, "\\subsection{Получившаяся производная:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree_deriv), 
                                                            tree_dtor(tree_deriv);tree_dtor(source);
    );

    TREE_ERROR_HANDLE(tree_simplify(&tree_deriv, flags_objs->out_file),           
                                                            tree_dtor(tree_deriv);tree_dtor(source);
    );

    fprintf(flags_objs->out_file, "\\subsection{Упростим её:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, tree_deriv), 
                                                            tree_dtor(tree_deriv);tree_dtor(source);
    );


    fprintf(flags_objs->out_file, "\\subsection{Графики:}\n"); 

    char source_filename[FILENAME_MAX] = {};

    if (!strncat(strncat(source_filename, flags_objs->out_filename, FILENAME_MAX),
                 "_graphic", FILENAME_MAX))
    {
        perror("Can't strncat graphic_filename");
                                                            tree_dtor(tree_deriv);tree_dtor(source);
        return EXIT_FAILURE;
    }

    TREE_ERROR_HANDLE(tree_create_graphic(source, tree_deriv, diff_var, source_filename, "graphic"),
                                                            tree_dtor(tree_deriv);tree_dtor(source);
    );

    fprintf(flags_objs->out_file, 
        "\n\\begin{figure}[h]\n"
            "\\centering\n"
            "\\includegraphics[width=0.8\\linewidth]{%s.png}\n"
        "\\end{figure}\n",
        source_filename
    );

                                                            tree_dtor(tree_deriv);tree_dtor(source);

    return FLAGS_ERROR_SUCCESS;
}

int mode_taylor(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    printf("Вы находитесь в режиме вычисления разложения тейлора\n");

    char diff_var = 'x';
    do
    {
        printf("Введите переменную дифферинцирования:\n");
        diff_var = (char)getchar();
    } 
    while (diff_var < 'a' || 'z' < diff_var || diff_var == 'e');

    operand_t diff_dot = 0.5;
    do
    {
        // scanf("%*[^\n]");
        printf("Введите точку, относительлно которой будет приближение:\n");
    } 
    while (scanf("%lf", &diff_dot) <= 0);
    // scanf("%lf", &diff_dot);

    // fprintf(stderr, "point: %lf\n", diff_dot);

    size_t diff_order = 0;
    do
    {
        scanf("%*[^\n]");
        printf("Введите порядок разложения:\n");
    } 
    while (scanf("%zu", &diff_order) <= 0);


    tree_t* source = NULL;
    TREE_ERROR_HANDLE(tree_read(flags_objs->in_filename, &source));

    fprintf(flags_objs->out_file, "\\subsection{Исходная функция:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, source), 
                                                                                  tree_dtor(source);
    );

    TREE_ERROR_HANDLE(tree_simplify(&source, flags_objs->out_file),           
                                                                                  tree_dtor(source);
    );

    fprintf(flags_objs->out_file, "\\subsection{Упростим её:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, source), 
                                                                                  tree_dtor(source);
    );

    tree_t* taylor = tree_taylor_polynom(source, diff_order, diff_var, diff_dot, 
                                         flags_objs->out_file);

    if (!taylor)
    {
        fprintf(stderr, "Can't tree_taylor_polynom\n");
                                                                                  tree_dtor(source);
        return EXIT_FAILURE;
    }

    fprintf(flags_objs->out_file, "\\subsection{Получившийся полином:}\n");

    TREE_ERROR_HANDLE(tree_print_tex(flags_objs->out_file, taylor), 
                                                                tree_dtor(taylor);tree_dtor(source);
    );


    fprintf(flags_objs->out_file, "\\subsection{Графики:}\n"); 

    char source_filename[FILENAME_MAX] = {};

    if (!strncat(strncat(source_filename, flags_objs->out_filename, FILENAME_MAX),
                 "_graphic", FILENAME_MAX))
    {
        perror("Can't strncat graphic_filename");
                                                                tree_dtor(taylor);tree_dtor(source);
        return EXIT_FAILURE;
    }

    TREE_ERROR_HANDLE(tree_create_graphic(source, taylor, diff_var, source_filename, "graphic"),
                                                                tree_dtor(taylor);tree_dtor(source);
    );

    fprintf(flags_objs->out_file, 
        "\n\\begin{figure}[h]\n"
            "\\centering\n"
            "\\includegraphics[width=0.8\\linewidth]{%s.png}\n"
        "\\end{figure}\n",
        source_filename
    );

                                                                tree_dtor(taylor);tree_dtor(source);

    return FLAGS_ERROR_SUCCESS;
}
int mode_test (flags_objs_t* const flags_objs)
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

    tree_t* tree2_ndiff = tree_ndiff(tree2, 5, 'x', stdout);

    tree_t* tree2_monom = tree_taylor_polynom(tree2, 1, 'x', 0.1, stdout);

    TREE_ERROR_HANDLE(tree_create_graphic(tree2_monom, NULL, 'x', "assets/polynom_tan", "tan"),
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