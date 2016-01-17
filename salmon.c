#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

//code size
#define n (6)			//Codeword length
#define d (3)			//Distance
#define words (400)
#define vocab (4096)     //(int)((double)pow ((double)4, (double)n));

// constants
#define Generation (20)
#define Population (400)
#define MemoryProbability (0.7)
#define ReproductionFraction (0.5)
#define RouletteSelection (0.5)
#define IM (5)


//set an integer array to a specific value
void set (int *arr, int val, int len)
{
	int i;
	for (i=0; i<len; ++i)
	{
		arr[i] = val;
	}
	return;
}


//hallo buon chan

//set a 2 d integer array to a specific value
void set2 (int *arr, int val, int len1, int len2)
{
	int i, j;
	for (i=0; i<len1; ++i)
	{
		for (j=0; j<len2; ++j)
		{
			arr[(i*len2)+j] = val;
		}
	}
	return;
}

//converting numbers to DNA
void convert (int dec, char *s)
{
	int i=0;
	int quat[n];													 //code length
	double q=0;
	
	//convert number base 10 to a number base 4
	while(i<n)
	{

         quat[i]= dec % 4;
		 q=q+((quat[i])* ((float) pow((float) 10,i)));
		 switch (quat[i])
		 {
			case 0:	s[n-i-1] = 'A';
				break;
			case 1:	s[n-i-1] = 'C';
				break;
			case 2:	s[n-i-1] = 'G';
				break;
			case 3:	s[n-i-1] = 'T';
				break;
		 }
		 i=++i;
		 dec = dec / 4;
	}
	return;
}


int main()
{
	//declare counters and variables
	int gen, salmon, memory, tabu, i;
	int un, can;
	int r;
	double random;
	double sum;
	double maxprob;
	double curprob;
	int probpos;
	int pop=Population;
	int temp;
	int margin;
	int max;
	int maxcan;

	//salmon algortihm variables
	int salmont[Population][words] = {0};
	int salmonm[Population][words] = {0};
	int unused[words] = {0};
	int candidates[words] = {0};
	int waterlevel[vocab] = {0};
	int salmong[Population]={0};
	int salmonsort[Population]={0};

	//lavensthein variables
	char word1[n] = {0};
	char word2[n] = {0};
	int matrix[n + 1][n + 1];
	int dlete;
	int insert;
	int substitute;
	int minimum;
	int min;

	//elite salmon
	int elitepos = 0;


	// Intializes random number generator 
	srand(time(NULL));


	set2 (&salmont[0][0], -1, Population, words);
	set2 (&salmonm[0][0], -1, Population, words);
	set (&unused[0], -1, words);
	set (&candidates[0], -1, words);
	set (&waterlevel[0], IM*words, vocab);



	//Initialise all salmon
	for (salmon=0; salmon<Population; ++salmon) 
	{
		memory =0;
		//place random clique in memory
		for (memory=0; memory < words; ++memory)
		{
			do
			{
				r = rand();
			} while (r >= powl(4,n));
			salmonm[salmon][memory] = r;
		}
	}



	// repeat for all generations of salmon
	for (gen=0; gen<Generation; ++gen) 
	{
		for (salmon=0; salmon<Population; ++salmon) 
		{
			tabu = 0;
			un = 0;
			can = 0;

			//take every vector in memory and check the probability that the vector is utilised by probability
			for (memory=0; memory< words; ++memory)
			{
				random=((double)rand()/(double)RAND_MAX);

				//if utilised the vector is stored in tabu list
				if ((salmonm[salmon][memory] >= 0) && (random<MemoryProbability)) 
				{
					salmont[salmon][tabu] = salmonm[salmon][memory];
					++tabu;
				}
				else
				{
					unused[un]= salmonm[salmon][memory];
					++un;
				}
			}

			//create candidates using the unused vectors
			for (un=0; un< words; ++un)
			//for (un=0; un< vocab; ++un)
			{ 
				min = 100;
				//converts unused vector to base4
				convert(un, &word1[0]);

				//calculate the distance of each possible candidate with the words present in the tabu list 
				for (memory=0; memory< words; ++memory)
				{
					//converts unused vector to base4
					convert(salmont[salmon][memory], &word2[0]);
					//Levensthein distance code taken from http://www.lemoda.net/c/levenshtein/ on 30/12/2015

					
					for (i = 0; i <= n; ++i) {
						matrix[i][0] = i;
					}
					for (i = 0; i <= n; ++i) {
						matrix[0][i] = i;
					}
					for (i = 1; i <= n; ++i) {
						int j;
						char c1;

						c1 = word1[i-1];
						for (j = 1; j <= n; ++j) {
							char c2;

							c2 = word2[j-1];
							if (c1 == c2) {
								matrix[i][j] = matrix[i-1][j-1];
							}
							else {

								dlete = matrix[i-1][j] + 1;
								insert = matrix[i][j-1] + 1;
								substitute = matrix[i-1][j-1] + 1;
								minimum = dlete;
								if (insert < minimum) {
									minimum = insert;
								}
								if (substitute < minimum) {
									minimum = substitute;
								}
								matrix[i][j] = minimum;
							}
						}
					}
					
					if (min>matrix[n][n])
					{
						min=matrix[n][n];
					}
				} 
				// checks if the vector is a candidate
				if ((unused[un] >= 0) && (min>=d))
				{
					candidates[can] = unused[un];
					++can;
				}
			}
			maxcan = can;

			do 
			{
				maxprob = 0; 
				probpos = 0;
				//choose a vector from candidate with the highest probability
				for (can=0; can< maxcan; ++can)
				{

					if (candidates[can] >=0)
					{
						//calculate Probability
						sum = 0;
						i =0;
						for (i=0;i<vocab;++i) 
						{
							sum = sum+((double)pow ((double)waterlevel[i], (double)RouletteSelection));
						}
	
						curprob = ((double)pow ((double)waterlevel[candidates[can]], (double)RouletteSelection)/sum);

						//compare probability with the highest probability
						if (curprob > maxprob)
						{
							maxprob=curprob;
							probpos=can; 
						}
					}
				}

				//select candidate with the highest probability and place it in tabu list
				salmont[salmon][tabu] = candidates[probpos];
				++tabu;
				//remove candiate from list
				i=0;
				for  (can=0; can < (words); ++can)
				{
					if (can != probpos)
					{
						candidates[i]=candidates[can];
						++i;
					}
				}
			} while (candidates[0] >=0);

		}
		
		//count the size of each salmon
		for (salmon=0; salmon<Population; ++salmon) 
		{
			i =0;
			for (tabu=0; tabu<words; ++tabu)
			{
				if (salmont[salmon][tabu]>=0)
				{
					++i;
				}
			}
			salmong[salmon] = i;
		}

		//update water levels
		for (salmon=0; salmon<pop; ++salmon) 
		{
			for (tabu=0; tabu<words; ++tabu)
			{
				if (salmont[salmon][tabu]>=0)
				{
					waterlevel[salmont[salmon][tabu]]+=salmong[salmon];
				}
			}
		}

		//duplicate array
		for (salmon = 0; salmon < pop; ++salmon)
		{
			salmonsort[salmon]=salmong[salmon];
		}

		//sorting salmon
		for (salmon = 0; salmon < pop; ++salmon)
		{
			for (i = salmon + 1; i < pop; ++i)
			{
				if (salmonsort[salmon] > salmonsort[i])
				{
					temp =  salmonsort[salmon];
					salmonsort[salmon] = salmonsort[i];
					salmonsort[i] = temp;
				}
			}
		}
		margin = (int)pop*ReproductionFraction;
		
		//reproduce
		set2 (&salmonm[0][0], -1, Population, words);
		max=salmonsort[pop-1];
		i=0;
		for (salmon = 0; salmon < pop; ++salmon)
		{
			if ((salmong[salmon]>=salmonsort[margin]) || (pop<10))				//allows a population of 10 salmon to survive after all generations 
			{
				//move tabu list to memory
				for (memory=0; memory<words; ++memory)
				{
					salmonm[i][memory]=salmont[salmon][memory];
				}
				if (salmong[salmon] == max)
				{
					elitepos = i;
				}
				++i;
			}
			else 
			{
				set (&salmonm[salmon][0], -1, words);
			}
		}


		if (gen == 19){
			pop = i;
		}
		//reset tabu list
		set2 (&salmont[0][0], -1, Population, words);
		set (&salmong[0], -1, Population);
		set (&salmonsort[0], -1, Population);
		pop = i; //update population

	}//loop for each generation



	min = 100;

	//check and print the code
	for (un=0; un < max; ++un)
	{ 
		//converts code  vector to base4
		convert(salmonm[elitepos][un], &word1[0]);

		//print codeword
		for (i=0; i<n; ++i)
		{
			printf("%c",word1[i]);
		}
		printf(" ");

		//loop through the other codes and checks their 
		for (memory=un+1; memory< max; ++memory)
		{
			//converts unused vector to base4
			convert(salmonm[elitepos][memory], &word2[0]);
			//Levensthein distance code taken from http://www.lemoda.net/c/levenshtein/ on 30/12/2015

					
			for (i = 0; i <= n; ++i) {
				matrix[i][0] = i;
			}
			for (i = 0; i <= n; ++i) {
				matrix[0][i] = i;
			}
			for (i = 1; i <= n; ++i) {
				int j;
				char c1;

				c1 = word1[i-1];
				for (j = 1; j <= n; ++j) {
					char c2;

					c2 = word2[j-1];
					if (c1 == c2) {
						matrix[i][j] = matrix[i-1][j-1];
					}
					else {

						dlete = matrix[i-1][j] + 1;
						insert = matrix[i][j-1] + 1;
						substitute = matrix[i-1][j-1] + 1;
						minimum = dlete;
						if (insert < minimum) {
							minimum = insert;
						}
						if (substitute < minimum) {
							minimum = substitute;
						}
						matrix[i][j] = minimum;
					}
				}
			}
			//print distance
			printf("%d ",matrix[n][n]);

			if (min>matrix[n][n])
			{
				min=matrix[n][n];
			}
		} 

		printf("\n");

	}
	// checks min distance
	if (min < d)
	{
		printf("\n Error in codebook");
	} else
	{
		printf("\n Biggest codebook has %d codes of length %d and minimum distance %d ",max, n, d);
	}

	return 0;
}
	


