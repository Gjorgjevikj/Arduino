/* Queue implementation
 *  FIFO structure to que events
 *  After filling up the que the new enque request will be ignored (and discarded)
 *  until a space is made available by calling deque 
 *  
 */
 
template <typename T, int N = 10, bool DO = false>
class Queue
{
public:
  Queue() : front(0), rear(0), size(0) { }

  // Stores something in the queue. 
  // Returns false if the buffer is full, true if stored sucessfully 
  // If Discard_Oldest mode selected (DO = true), always returns true and overwrites the oldest records
  bool enqueue(T val)
  {
    if (size == N)
    {
      if (DO)
      {
        front++;
        front %= N;
        size--;
      }
      else
        return false;
    }
    buffer[rear] = val;
    rear++;
    rear %= N;
    size++;
    return true;
  }

  // Return number of elements waiting in the queue
  int waiting() const
  {
    return size; //rear - front + (front > rear ? N : 0);
  }

  // Retrieve something from the queue. 
  // Returnes 0 if the queue is empty
  T dequeue()
  {
    if (size == 0)
      return 0;
    T val(buffer[front]);
    front++;
    front %= N;
    size--;
    return val;
  }

private:
  volatile int size;
  volatile int front; // points @ the item before the one to be dequed
  volatile int rear; // points @ the item that has been enqued last
  volatile T buffer[N];
};

