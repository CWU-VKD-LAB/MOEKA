#include "HanselChains.h"
#include <utility>  // std::move
#include <algorithm> // shrink_to_fit

using Chain3D = std::vector<std::vector<std::vector<int>>>;

// Now passing 'chain' by reference to avoid an initial copy.
Chain3D HanselChains::genChains(int num, int vector_dimension, std::unordered_map<int, Chain3D>& chain)
{
    // Use the passed‑in reference directly (alias as 'chains' for clarity)
    auto& chains = chain;

    /* -------------------------------------------------------------------
       add key string "0","1",...,num to the beginning of each element
       in the corresponding key
       -------------------------------------------------------------------*/
    for (int j = 0; j < num; ++j)
    {
        auto& cj = chains.at(j);
        for (int k = 0; k < static_cast<int>(cj.size()); ++k)
        {
            auto& cjk = cj[k];

            // Re‑use a single scratch buffer instead of reallocating each loop
            std::vector<int> scratch(vector_dimension);

            for (int l = 0; l < static_cast<int>(cjk.size()); ++l)
            {
                // *** build the transformed vector ***
                scratch[vector_dimension - 1] = j;
                for (int i = vector_dimension - 1; i >= 1; --i)
                    scratch[i - 1] = cjk[l][i];          // FIX preserved

                // move scratch into target – no extra copy
                cjk[l] = std::move(scratch);

                // restore capacity without realloc
                scratch.clear();
                scratch.resize(vector_dimension);
            }
        }
    }

    /* -------------------------------------------------------------------
       move the largest of each sub vector …
       -------------------------------------------------------------------*/
    for (int j = 0; j < static_cast<int>(chains.size()) - 1; ++j)
    {
        auto& cj = chains.at(j);

        for (int k = 0; k < static_cast<int>(cj.size()); ++k)
        {
            int count = j;
            for (count; count < static_cast<int>(chains.size()) - 1; ++count)
            {
                if (count + 1 >= static_cast<int>(chains.size()))
                    break;

                if (k >= static_cast<int>(chains.at(count + 1).size()))
                    break;

                auto& nextVec = chains.at(count + 1)[k];
                if (nextVec.empty())
                    break;

                // move last element from nextVec to current vec
                cj[k].push_back(std::move(nextVec.back()));
                nextVec.pop_back();
            }
        }
    }

    /* -------------------------------------------------------------------
       we need to prepare for the next iteration of i in the first for loop
       by adding all chains together, then adding combined chains to each key
       -------------------------------------------------------------------*/
    for (int j = 1; j < num; ++j)
    {
        for (auto& sub : chains.at(j))
            chains.at(0).push_back(sub);   // copy unavoidable by design
    }

    for (int j = 1; j < num; ++j)
        chains.at(j) = chains.at(0);       // duplicates for later independence

    // Return the combined chains (using the first key)
    return chains.at(0);
}


//TODO: fix num call, since this seems to be how we are assigning things to k values instead of target classes
// There seems to be an issue in how we are generating the chains themselves.

void HanselChains::calculateHanselChains(int vector_dimension)
{
    // For n dimensions, iterate through to generate chains
    for (int dim_num = 0; dim_num < vector_dimension; ++dim_num)
    {
        // num is the k value of a particular attribute
        // PROBABLY AN ISSUE
        int num = attributes[dim_num];

        // Need a base chain to use for genChains
        if (dim_num == 0)
        {
            // generate the base chain
            std::vector<int> scratch(vector_dimension);
            std::vector<std::vector<int>> baseChain;
            baseChain.reserve(num);                     // OPT: reserve once

            // hansel chains is 3d, first dimension is one particular number in the chain
            // second is one chain
            // third is the whole set of chains

            // k is the k value of a particular attribute
            for (int k = 0; k < num; ++k)
            {
                scratch.assign(vector_dimension, 0);
                scratch[vector_dimension - 1] = k;
                baseChain.push_back(scratch);           // copy small vector
            }

            // create a vector of vectors to hold the baseChain
            hanselChainSet.push_back(std::move(baseChain));
        }
        else
        {
            // Create a local map where each key gets the current hanselChainSet.
            std::unordered_map<int, Chain3D> chainNum;
            chainNum.reserve(num);                      // OPT: avoid rehash

            for (int k = 0; k < num; ++k)
                chainNum.emplace(k, hanselChainSet);    // deep copy required

            // Call genChains with the map passed by reference
            hanselChainSet = genChains(num, vector_dimension, chainNum);

            // Explicitly free map memory early
            chainNum.clear();
            chainNum.rehash(0);
        }
    }

    // Remove any empty chains from the end
    for (int i = static_cast<int>(hanselChainSet.size()) - 1; i >= 0; --i)
    {
        if (hanselChainSet[i].empty())
            hanselChainSet.erase(hanselChainSet.begin() + i);
    }
}
