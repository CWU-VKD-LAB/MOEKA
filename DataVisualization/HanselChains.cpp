#include "HanselChains.h"

// Now passing 'chain' by reference to avoid an initial copy.
std::vector<std::vector<std::vector<int>>> HanselChains::genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<std::vector<int>>>>& chain){
    // Use the passed-in reference directly (alias as 'chains' for clarity)
    auto& chains = chain;

    // add key string "0","1",...,num to the beginning of each element in the corresponding key
    for (int j = 0; j < num; j++)
    {
        for (int k = 0; k < chains.at(j).size(); k++)
        {
            for (int l = 0; l < chains.at(j)[k].size(); l++)
            {
                std::vector<int> b(vector_dimension);
                b[vector_dimension - 1] = j;

                for (int i = vector_dimension - 1; i >= 1; i--)
                {
                    // FIX
                    b[i - 1] = chains.at(j)[k][l][i]; // used to be for std::vector: b[i] = chains.at(j)[k][l][i - 1];
                }

                std::vector<int> c = b; // copy b into c
                chains.at(j)[k][l] = c;
            }
        }
    }

    // move the largest of each sub vector to corresponding sub vector in the first key
    // then move the largest of each subsequent sub vector to the second key's sub vector
    // repeat until chain.at(num-2) is reached since the last key will have nothing moved to it
    // j = current key being added to
    // emptV = number of empty vectors
    for (int j = 0; j < chains.size() - 1; j++)
    {
        for (int k = 0; k < chains.at(j).size(); k++)
        {
            // I think I need to make the code below recursive
            int count = j;

            for (count; count < chains.size() - 1; count++)
            {
                if (count + 1 >= chain.size())
                {
                    break;
                }

                /// USE FIND AND END because kth vector doesn't exist for 3d 3val, k=2
                if (k > chains.at(count + 1).size() - 1)
                {
                    break;
                }

                if (chains.at(count + 1).at(k).empty())
                {
                    // ### DEAL WITH EMPTY vectors~!###
                    // chains.at(to_string(count + 1)).at(k).erase();
                    break;
                }

                int end = (int)chains.at(count + 1).at(k).size();

                // add last element from position k of key count+1 to position k of j
                chains.at(j)[k].push_back(chains.at(count + 1)[k].at(end - 1));

                // pop off the moved element
                chains.at(count + 1).at(k).pop_back();
            }
        }
    }

    // we need to prepare for the next iteration of i in the first for loop
    // by adding all chains together, then adding combined chains to each key
    for (int j = 1; j < num; j++)
    {
        for (int k = 0; k < chains.at(j).size(); k++)
        {
            chains.at(0).push_back(chains.at(j)[k]);
        }
    }

    for (int j = 0; j < num; j++)
    {
        chains.at(j) = chains.at(0);
    }

    // Return the combined chains (using the first key)
    std::vector<std::vector<std::vector<int>>> chainsTogether = chains.at(0);
    return chainsTogether;
}


//TODO: fix num call, since this seems to be how we are assigning things to k values instead of target classes
// There seems to be an issue in how we are generating the chains themselves.

void HanselChains::calculateHanselChains(int vector_dimension)
{
    // For n dimensions, iterate through to generate chains
    for (int dim_num = 0; dim_num < vector_dimension; dim_num++)
    {
        // num is the k value of a particular attribute
        // PROBABLY AN ISSUE
        int num = attributes[dim_num];

        // Need a base chain to use for genChains
        if (dim_num == 0)
        {
            // generate the base chain
            std::vector<std::vector<int>> baseChain;

            // hansel chains is 3d, first dimension is one particular number in the chain
            // second is one chain
            // third is the whole set of chains

            // k is the k value of a particular attribute
            for (int k = 0; k < num; k++)
            {
                std::vector<int> b(vector_dimension);
                b[vector_dimension - 1] = k;
                std::vector<int> c = b; // copy b into c
                // b.setLevel();
                baseChain.push_back(c);
            }

            // create a vector of vectors to hold the baseChain
            hanselChainSet.push_back(baseChain);
        }
        else
        {
            // Create a local map where each key gets the current hanselChainSet.
            std::unordered_map<int, std::vector<std::vector<std::vector<int>>>> chainNum;
            for (int k = 0; k < num; k++)
            {
                chainNum[k] = hanselChainSet;
            }

            // Call genChains with the map passed by reference
            hanselChainSet = genChains(num, vector_dimension, chainNum);
        }
    }

    // Remove any empty chains from the end
    for (int i = (int)hanselChainSet.size() - 1; i >= 0; i--)
    {
        if (!hanselChainSet[i].size())
        {
            hanselChainSet.erase(hanselChainSet.begin() + i);
        }
    }
}
