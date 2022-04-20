#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <queue>
#include <exception>
#include <cmath>

using namespace std;

class RangeMaxSegmentTreeException : public exception
{
private:
    string d_msg;
public:
    RangeMaxSegmentTreeException(const string& err_msg = "Query out of bound") : d_msg(err_msg) {}
    const char* what() const noexcept override{
        return d_msg.c_str();
    }
};

class RangeMaxSegmentTree
{
private:
    vector<int> d_st; // segment tree
    int  d_size; // number of element of input raw array
    vector<int> d_raw; // raw array, of which the elements can be updated
private:
    int constructSegmentTreeUtil(const vector<int>& nums, int left, int right, int st_idx)
    {
        if(left == right)
        {
            d_st[st_idx] = nums[left];    
            return nums[left];
        }
        int mid = (left + right) / 2;
        d_st[st_idx] = max(constructSegmentTreeUtil(nums, left, mid, 2*st_idx+1), constructSegmentTreeUtil(nums, mid + 1, right, 2 * st_idx + 2));
        return d_st[st_idx];
    }
    int getMaxUtil(int q_left, int q_right, int st_idx, int node_left, int node_right) // node_left, node_right -> raw array range index for this node
    {
       // this node range is out of query range
       if(q_left > node_right || q_right < node_left)
       {
            return INT_MIN;
       }
       // this node range fall within query range
       if(q_left <= node_left && q_right >= node_right)
       {
            return d_st[st_idx];
       }
       // part of this node range fall within query range
       int mid = (node_left + node_right) / 2;
       return max(getMaxUtil(q_left, q_right, 2*st_idx + 1, node_left, mid), getMaxUtil(q_left, q_right, 2*st_idx+2, mid+1, node_right));
    }
    void updateValueUtil(int i, int new_val, int st_idx, int node_left, int node_right)
    {
        // i is out of node range
       if( i < node_left || i > node_right)
       {
            return;
       }
       d_st[st_idx] = max(d_st[st_idx], new_val);
       // otherwise we will go out of bound 
       if(node_left != node_right)
       {
            int mid = (node_left + node_right) / 2;
            updateValueUtil(i, new_val, 2*st_idx+1, node_left, mid);
            updateValueUtil(i, new_val, 2*st_idx+2, mid+1, node_right);
       }
    }
public:
    void constructSegmentTree(const vector<int>& nums)
    {
        int x = (int)(ceil(log2(nums.size())));
        x = (1 << x);
        d_st.resize(2*x - 1, INT_MIN);
        d_size = nums.size();
        constructSegmentTreeUtil(nums, 0, d_size - 1, 0);
        d_raw = nums;
    }
    int getMax(int q_left, int q_right)
    {
        if(q_left < 0 || q_right > d_size - 1 || q_right < q_left)
        {
            throw RangeMaxSegmentTreeException("getMax(" + to_string(q_left) + ", " + to_string(q_right) + ")" );
        }
        return getMaxUtil(q_left, q_right, 0, 0, d_size - 1);
    }
    void updateValue(int i, int new_val)
    {
        if(i < 0 || i > d_size - 1)
        {
            throw RangeMaxSegmentTreeException("updateValue(" + to_string(i) + ", " + to_string(new_val) + ")");
        }
        d_raw[i] = new_val;
        updateValueUtil(i, new_val, 0, 0, d_size - 1);
    }
};

int main()
{
    vector<int> nums = {2,5,1,4,9,3};
    RangeMaxSegmentTree obj;
    obj.constructSegmentTree(nums);
    cout << obj.getMax(1,4) << endl;;
    obj.updateValue(2, 10);
    cout << obj.getMax(1,4) << endl;;
    try
    {
        obj.getMax(1,7);
    }
    catch(const exception& e)
    {
        cout << e.what() << endl;
    }

    try
    {
        obj.updateValue(7,7);
    }
    catch(const exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}

