#ifndef SISO2_BLOCK_INTERLEAVER_H
#define SISO2_BLOCK_INTERLEAVER_H

#include <stdint.h>
#include <vector>

namespace SISO2
{

template<size_t BLOCK_SIZE>
class BlockInterleaver
{
public:
    template<typename T>
    auto operator()(const std::vector<T>& in) const
    {
        std::vector<T> out(in.size());

        const int col_num = BLOCK_SIZE;
        const int row_num = (int)(in.size()/col_num);
        const int residue = in.size() - col_num*row_num;
        const T* ptr[row_num+1];
        ptr[0] = in.data();
        for (int j=1; j<row_num+1; j++)
            ptr[j] = ptr[j-1] + col_num;
        
        int k = 0;
        for (int i=0; i<residue; i++)
            for (int j=0; j<row_num+1; j++)
            {
                out[k++] = *ptr[j];
                ptr[j] += 1;
            }
                
        for (int i=residue; i<col_num; i++)
            for (int j=0; j<row_num; j++)
            {
                out[k++] = *ptr[j];
                ptr[j] += 1;
            }
        return out;
    }
};

template<size_t BLOCK_SIZE>
class BlockDeinterleaver
{
public:
    template<typename T>
    auto operator()(const std::vector<T>& in) const
    {
        std::vector<T> out(in.size());

        const int col_num = BLOCK_SIZE;
        const int row_num = (int)(in.size()/col_num);
        const int residue = in.size() - col_num*row_num;
        if (residue == 0) {
            const T* ptr[col_num];
            ptr[0] = in.data();
            for (int j=1; j<col_num; j++)
                ptr[j] = ptr[j-1] + row_num;

            for (int i=0, k=0; i<row_num; i++)
                for (int j=0; j<col_num; j++)
                {
                    out[k++] = *ptr[j];
                    ptr[j] += 1;
                }
        }
        else {
            const T* ptr[col_num];
            ptr[0] = in.data();
            for (int j=1; j<=residue; j++)
                ptr[j] = ptr[j-1] + (row_num+1);
            for (int j=residue+1; j<col_num; j++)
                ptr[j] = ptr[j-1] + row_num;

            int k = 0;
            for (int i=0; i<row_num; i++)
                for (int j=0; j<col_num; j++)
                {
                    out[k++] = *ptr[j];
                    ptr[j] += 1;
                }
            for (int j=0; j<residue; j++)
            {
                out[k++] = *ptr[j];
                ptr[j] += 1;
            }
        }
        return out;
    }
};

}

#endif

