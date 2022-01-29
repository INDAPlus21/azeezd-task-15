#include<queue>
#include<stack>
#include<iostream>

int main()
{
    // Values given from inputs
    size_t amount, cmd_num, val;

    // Loops once for each test case stops once EOF
    while(std::cin >> amount) {

        // Structures to investigate
        std::priority_queue<int> p_queue;
        std::stack<int> stack;
        std::queue<int> queue;

        // Program assume all is true in the beginning, then deduces the falsehoods from given data
        bool is_stk = true, is_q = true, is_pq = true;

        // Loops for each operation
        for (size_t i = 0; i < amount; i++)
        {
            std::cin >> cmd_num >> val;
            switch (cmd_num)
            {
            case 1: // PUSH operation
                p_queue.push(val);
                stack.push(val);
                queue.push(val);
                break;
            case 2: // POP operation

                // If trying to pop empty structures, then that is impossible
                if(stack.size() == 0) {is_stk = 0; is_q = 0; is_pq = 0; continue;}

                // Pop value from each structure and compare it to the output changing the truth values
                is_stk &= stack.top() == val;
                stack.pop();
                
                is_q &= queue.front() == val;
                queue.pop();

                is_pq &= p_queue.top() == val;
                p_queue.pop();
                break;
            default:
                break;
            }
        }

        // Three bits controls flags. 000 = Stack|Queue|PQueue
        int flags = is_stk << 2 | is_q << 1 | is_pq;
        
        /* 
        000 = nothing is true, impossible. 
        100 010 001, single truth, look at flags variable above
        110 011 101 111, multiple truths, not sure
        */
        switch (flags)
        {
        case 0:
            std::cout << "impossible\n";
            break;
        case 1:
            std::cout << "priority queue\n";
            break;
        case 2:
            std::cout << "queue\n";
            break;
        case 4:
            std::cout << "stack\n";
            break;
        default:
            std::cout << "not sure\n";
            break;
        }
    }

    return 0;
}