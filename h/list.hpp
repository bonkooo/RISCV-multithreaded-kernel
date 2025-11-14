#ifndef LIST_HPP
#define LIST_HPP

template<typename T>
class List{
private:
    struct Node{
        T *data;
        Node *next;
        Node(T* data, Node* next) : data(data), next(next) {}
    };

    Node *head;
    Node *tail;
public:
    List(): head(0), tail(0) {}
    List(const List<T> &) = delete; // obrisi kopirajuci konstruktor
    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T* data){
        Node* newNode = new Node(data, head);
        head = newNode;
        if (!tail) tail = newNode;
    }

    void addLast(T* data){
        Node* newNode = new Node(data, 0);
        if (tail){
            tail->next = newNode;
            tail = tail->next;
        }
        else{
            head = newNode;
            tail = newNode;
        }
    }

    T* removeFirst(){
        if (!head) return 0;
        Node* node = head;
        head = head->next;
        if (!head) tail = 0;

        T* ret = node->data;
        delete node;
        return ret;
    }

    T* peekFirst(){
        if (!head) return 0;
        Node* node = head;
        T* ret = node->data;
        return ret;
    }

    T* removeLast(){
        if (!head) return 0;

        Node* prev = 0;
        for (Node* curr = head; curr && curr != tail; curr = curr->next){
            prev = curr;
        }

        Node* node= tail;
        if (prev){
            prev->next = 0;
        }
        else{
            head = 0;
        }
        tail = prev;

        T* ret = node->data;
        delete node;
        return ret;
    }

    T* peekLast(){
        if (!tail) return 0;
        return tail->data;
    }
};

#endif





























