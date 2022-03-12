#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>

#define dice_max    6

//#define max_score (dice_max * 2)

int n_dice_combo;

bool verbose = false;

void count_score_recur (int depth, int max_depth, int max_score, int *guesses, int &max_score_found, int *best_guesses, bool **covers)
{   int iguess;
	int iguess_start;
	int this_score;
	int idepth;
	int combo_index;
	bool good_guess;

	if (depth == max_depth)
	{	this_score = 0;
		for (combo_index = 0; combo_index < n_dice_combo; combo_index++)
		{   good_guess = false;
			for (idepth = 0; idepth < max_depth; idepth++)
			{   if (covers [combo_index] [guesses [idepth]])
				{   good_guess = true;
				}
			}
			if (good_guess)
			{   this_score++;
			}
		}
		if (verbose)
		{	printf ("test ");
			for (idepth = 0; idepth < max_depth; idepth++)
			{   printf (" %d", guesses [idepth]);
			}
			printf (" score %d\n", this_score);
        }
		if (this_score > max_score_found)
		{	for (idepth = 0; idepth < max_depth; idepth++)
			{   best_guesses [idepth] = guesses [idepth];
            }
			max_score_found = this_score;
		}
	} else
	{	 if (depth == 0)
		{   iguess_start = 2;
		} else
		{	iguess_start = guesses [depth - 1] + 1;
		}
		for (guesses [depth] = iguess_start; guesses [depth] <= max_score; guesses [depth]++)
		{	count_score_recur (depth + 1, max_depth, max_score, guesses, max_score_found, best_guesses, covers);
		}
	}


}

void play_depth (int nguesses, int ndice, int n_dice_per_sum, int max_score, bool **covers)
{   int *guesses;
	int *best_guesses;
	int best_score;
	int idepth;

	guesses = new int [nguesses];
	best_guesses = new int [nguesses];
    best_score = 0;
	count_score_recur (0, nguesses, max_score, guesses, best_score, best_guesses, covers);
	printf ("play %d guesses %d dice %d dice_per_sum best ", nguesses, ndice, n_dice_per_sum);
	for (idepth = 0; idepth < nguesses; idepth++)
	{   if (idepth > 0)
		{	printf (",");
		}
		printf ("%d", best_guesses [idepth]);
	}
	printf (" score %d\n", best_score);


	delete[] guesses;
	delete[] best_guesses;

}

void gen_sums_recur (int partial_sum, int depth, int max_depth, int idie_low, int n_dice, int *dice_vals, bool *has_val)
{   int idie;

	if (depth == max_depth)
	{   has_val [partial_sum] = true;
	} else
	{	for (idie = idie_low; idie < n_dice; idie++)
		{   partial_sum += dice_vals [idie];
			gen_sums_recur (partial_sum, depth + 1, max_depth, idie + 1, n_dice, dice_vals, has_val);
			partial_sum -= dice_vals [idie];
		}
	}

}

void gen_dice_rolls_recur (int depth, int max_depth, int *dice_vals, int dice_per_sum, bool **combo_covers)
{   int icombo;
	int idie;

	if (depth == max_depth)
	{   icombo = 0;
		for (idie = 0; idie < max_depth; idie++)
		{	icombo = icombo * dice_max + (dice_vals [idie] - 1);
		}
		gen_sums_recur (0, 0, dice_per_sum, 0, max_depth, dice_vals, combo_covers [icombo]);
	} else
	{	for (idie = 1; idie <= dice_max; idie++)
		{   dice_vals [depth] = idie;
			gen_dice_rolls_recur (depth + 1, max_depth, dice_vals, dice_per_sum, combo_covers);
		}
    }

}

void play (int nguesses, int ndice, int n_dice_per_sum)
{   int *dice_vals;
	int max_score_found;
	bool **combo_covers;
	int combo_index;
	int score;
	int icover;
	int *total_combos;
	int iscore;
	int i;
	int max_score;


	max_score = n_dice_per_sum * dice_max;
	total_combos = new int [max_score + 1];


	n_dice_combo = 1;
	for (i = 0; i < ndice; i++)
	{   n_dice_combo = n_dice_combo * dice_max;
	}

	combo_covers = new bool * [n_dice_combo];
	dice_vals = new int [ndice];

	for (combo_index = 0; combo_index < n_dice_combo; combo_index++)
	{   combo_covers [combo_index] = new bool [max_score + 1];
		for (icover = 0; icover < max_score + 1; icover++)
		{   combo_covers [combo_index] [icover] = false;
		}
	}

	/* build a table listing every possible valid sum of 2 dice out of a roll consisting of 4 dice.
	 * Simply iterate through all possible combinations of 4 dice, and each of the 6 possible sums.
	 * Build a boolean array that has a true entry if the corresponding sum is possible
	 */

	gen_dice_rolls_recur (0, ndice, dice_vals, n_dice_per_sum, combo_covers);

#ifdef foo
	combo_index = 0;
	for (dice_vals [0] = 1; dice_vals [0] <= dice_max; dice_vals [0]++)
	{	for (dice_vals [1] = 1; dice_vals [1] <= dice_max; dice_vals [1]++)
		{	for (dice_vals [2] = 1; dice_vals [2] <= dice_max; dice_vals [2]++)
			{	for (dice_vals [3] = 1; dice_vals [3] <= dice_max; dice_vals [3]++)
				{   /* set entries of combo_covers to true for the 6 possible sums */
					combo_covers [combo_index] [dice_vals [0] + dice_vals [1]] = true;
					combo_covers [combo_index] [dice_vals [0] + dice_vals [2]] = true;
					combo_covers [combo_index] [dice_vals [0] + dice_vals [3]] = true;
					combo_covers [combo_index] [dice_vals [1] + dice_vals [2]] = true;
					combo_covers [combo_index] [dice_vals [1] + dice_vals [3]] = true;
					combo_covers [combo_index] [dice_vals [2] + dice_vals [3]] = true;
					combo_index++;
                }
			}
		}
	}
#endif

	play_depth (nguesses, ndice, n_dice_per_sum, max_score, combo_covers);

	for (iscore = 0; iscore <= max_score; iscore++)
	{   total_combos [iscore] = 0;
	}
	for (combo_index = 0; combo_index < n_dice_combo; combo_index++)
	{   for (iscore = 0; iscore <= max_score; iscore++)
		{   total_combos [iscore] += combo_covers [combo_index] [iscore];
		}
	}
	#ifdef print_covers
	printf ("covers");
	for (iscore = 0; iscore <= max_score; iscore++)
	{   printf (" %d", total_combos [iscore]);
	}
	printf ("\n");
	#endif


	delete [] dice_vals;
	delete [] total_combos;
	for (combo_index = 0; combo_index < n_dice_combo; combo_index++)
	{   delete [] combo_covers [combo_index];
	}
	delete [] combo_covers;


}

int _tmain(int argc, _TCHAR* argv[])
{   int nguesses;
	int ndice;
	int n_dice_per_sum;

	sscanf (argv [1], "%d", &nguesses);
	sscanf (argv [2], "%d", &ndice);
	sscanf (argv [3], "%d", &n_dice_per_sum);

	play (nguesses, ndice, n_dice_per_sum);
	return 0;
}
