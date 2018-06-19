//
//  btree.h
//  HW-7
//
//  Created by Abhishek Venkatesh on 5/21/18.
//  Copyright © 2018 Abhishek Venkatesh. All rights reserved.
//

#ifndef btree_h
#define btree_h
template <typename T, int M>
class BTree;

template <typename T,int M>
class BTreeNode {
    friend class BTree<T,M>;
public:
    BTreeNode() {
        keyTally = 0;
    };
    BTreeNode(const T& el) {
        keyTally = 1;
        keys[0] = el;
    };
    void snapShot(std::ostream& out);
private:
    bool leaf;
    int keyTally;
    T keys[M-1];
    BTreeNode* pointers[M];
};

template <typename T, int M>
class BTree {
public:
    BTree() { root = new BTreeNode<T, M>();}
    void snapShot(std::ostream& out);
    void insert(const T& key);
    void BTreeDelete(const T& key) { BTreeDelete(root, key);}
    void BTreeMerge(const T& key) { BTreeMerge(root, key);}
    void traversal() { traversal(root);}
    BTreeNode<T, M>* findParent(const T& key) {
        return findParent(root, nullptr, key);
    }
private:
    BTreeNode<T, M>* root;
    void BTreeDelete(BTreeNode<T, M>* &mover, const T& key);
    void BTreeMerge(BTreeNode<T, M>* &node, int index);
    void traversal(BTreeNode<T, M>* &node);
    BTreeNode<T, M>* findParent(BTreeNode<T, M>* node, BTreeNode<T, M>* parent, const T& key);
};

template <typename T, int M>
void BTreeNode<T, M>::snapShot(std::ostream& out) {
    out << '\"' << keys[0];
    for (int n = 1; n < keyTally; n++) {
        out << ':' << keys[n];
    }
    out << '\"';
    if (!leaf) {
        out << '[';
        pointers[0]->snapShot(out);
        for (int n = 1; n <= keyTally; n++) {
            out << ',';
            pointers[n]->snapShot(out);
        }
        out << ']';
    }
}
template <typename T, int M>
void BTree<T, M>::snapShot(std::ostream& out) {
    out << "TreeForm[";
    root->snapShot(out);
    out << ']' << std::endl;
}

template <typename T, int M>
BTreeNode<T, M>* BTree<T, M>::findParent(BTreeNode<T, M>* node, BTreeNode<T, M>* parent, const T&key) {
    int m = 0;
    while(m < node->keyTally && key > node->keys[m]) {
        m++;
    }
    if (m < node->keyTally && node->keys[m] == key) {
        return parent;
    }
    return findParent(node->pointers[m], node, key);
    
}

template <typename T, int M>
void BTree<T, M>::insert(const T& key) {
    BTreeNode<T, M>* mover = root;
    BTreeNode<T, M>* parent = nullptr;
    while(mover->keyTally == M-1 || !mover->leaf) {
        if (mover->keyTally == M-1) {
            if (mover == root) {
                parent = new BTreeNode<T, M>();
                parent->leaf = false;
                parent->pointers[0] = mover;
                root = parent;
            }
            BTreeNode<T, M>* sibling = new BTreeNode<T, M>();
            int middle = ceil(((double)M)/2 - 1);
            int j;
            for (j = parent->keyTally; j > 0; j--) {
                if (parent->keys[j-1] > mover->keys[middle]) {
                    parent->keys[j] = parent->keys[j-1];
                    parent->pointers[j+1] = parent->pointers[j];
                } else {
                    break;
                }
            }
            parent->keys[j] = mover->keys[middle];
            parent->keyTally++;
            parent->pointers[j+1] = sibling;
            int k;
            for (k = middle + 1; k < mover->keyTally; k++) {
                sibling->keys[sibling->keyTally] = mover->keys[k];
                sibling->pointers[sibling->keyTally] = mover->pointers[k];
                sibling->keyTally++;
            }
            sibling->pointers[sibling->keyTally] = mover->pointers[k];
            mover->keyTally = middle;
            if (mover->leaf) {
                sibling->leaf = true;
                mover = parent;
                int m = 0;
                while(key > mover->keys[m]) {
                    if (m == mover->keyTally) {
                        break;
                    }
                    m++;
                }
                if (key == mover->keys[m]) {
                    return;
                }
                mover = mover->pointers[m];
                break;
            }
        } else {
            int m = 0;
            while(key > mover->keys[m]) {
                if (m == mover->keyTally) {
                    break;
                }
                m++;
            }
            if (key == mover->keys[m]) {
                return;
            }
            parent = mover;
            mover = mover->pointers[m];
        }
    }
    int i;
    for (i = mover->keyTally; i > 0; i--) {
        if (mover->keys[i-1] > key) {
            mover->keys[i] = mover->keys[i-1];
        } else {
            break;
        }
    }
    mover->keys[i] = key;
    mover->keyTally++;
}

template <typename T, int M>
void BTree<T, M>::BTreeMerge(BTreeNode<T, M>* &node, int index) {
    //BTreeNode<T, M>* leftChild = node->pointers[index];
    //BTreeNode<T, M>* rightChild = node->pointers[index+1];
    node->pointers[index]->keys[node->pointers[index]->keyTally] = node->keys[index];
    node->pointers[index]->keyTally++;
    for (int i = 0; i < node->pointers[index+1]->keyTally; i++) {
        node->pointers[index]->keys[node->pointers[index]->keyTally] = node->pointers[index+1]->keys[i];
        node->pointers[index]->pointers[node->pointers[index]->keyTally] = node->pointers[index+1]->pointers[i];
        node->pointers[index]->keyTally++;
    }
    node->pointers[index]->pointers[node->pointers[index]->keyTally] = node->pointers[index+1]->pointers[node->pointers[index+1]->keyTally];
    node->pointers[index+1] = nullptr;
    if (node == root && node->keyTally == 1) {
        root = node->pointers[index];
        node = root;
        return;
    }
    for (int i = index+1; i < node->keyTally; i++) {
        node->keys[i-1] = node->keys[i];
        node->pointers[i] = node->pointers[i+1];
    }
    node->pointers[node->keyTally] = nullptr;
    node->keyTally--;
    node->keys[node->keyTally] = 0;
}

template <typename T, int M>
void BTree<T, M>::BTreeDelete(BTreeNode<T, M>* &mover, const T& key) {
    int i = 0;
    while(key > mover->keys[i] && i < mover->keyTally) {
        i++;
    }
    int a = i;
    if (mover->leaf) {
        if (mover->keys[i] != key) {
            return;
        } else {
            for (int j = i; j < mover->keyTally-1; j++) {
                mover->keys[j] = mover->keys[j+1];
            }
            mover->keyTally--;
            mover->keys[mover->keyTally] = 0;
            return;
        }
    } else {
        if (mover->keys[i] != key) {
            BTreeDelete(mover->pointers[i], key);
        } else {
            BTreeNode<T, M>* newMover = mover->pointers[i];
            BTreeNode<T, M>* start = newMover;
            while(!newMover->leaf) {
                newMover = newMover->pointers[newMover->keyTally];
            }
            T largestKey = newMover->keys[newMover->keyTally-1];
            mover->keys[i] = largestKey;
            BTreeDelete(newMover, largestKey);
            newMover = start;
            while (newMover->keyTally <= ceil(((double)M)/2) && newMover != mover && !newMover->leaf) {
                i = newMover->keyTally;
                BTreeNode<T, M>* leftChild = newMover->pointers[i];
                BTreeNode<T, M>* rightChild = newMover->pointers[i+1];
                if (i > 0 && (newMover->pointers[i]->keyTally + newMover->pointers[i-1]->keyTally + 1 <= M-1) && newMover->keyTally > 1) {
                    BTreeMerge(newMover, i-1);
                }
                if (i < newMover->keyTally && (newMover->pointers[i]->keyTally + newMover->pointers[i+1]->keyTally + 1 <= M-1) && newMover->keyTally > 1){
                    BTreeMerge(newMover, i);
                }
                if (i > 0) {
                    while(newMover->pointers[i]->keyTally < ceil(((double)M)/2 - 1)) {
                        newMover->pointers[i]->keys[newMover->pointers[i]->keyTally] = newMover->pointers[i]->keys[newMover->pointers[i]->keyTally-1];
                        newMover->pointers[i]->keys[newMover->pointers[i]->keyTally-1] = newMover->keys[i-1];
                        newMover->pointers[i]->keyTally++;
                        for (int j = newMover->pointers[i]->keyTally; j > 0; j--) {
                            newMover->pointers[i]->pointers[j] = newMover->pointers[i]->pointers[j-1];
                        }
                        newMover->keys[i-1] = newMover->pointers[i-1]->keys[newMover->pointers[i-1]->keyTally-1];
                        newMover->pointers[i]->pointers[0] = newMover->pointers[i-1]->pointers[newMover->pointers[i-1]->keyTally];
                        newMover->pointers[i-1]->pointers[newMover->pointers[i-1]->keyTally] = nullptr;
                        newMover->pointers[i-1]->keyTally--;
                        newMover->keys[newMover->keyTally] = 0;
                    }
                }
                if (i < newMover->keyTally) {
                    while(newMover->pointers[i]->keyTally < ceil(((double)M)/2 - 1)) {
                        newMover->pointers[i]->keys[newMover->pointers[i]->keyTally] = newMover->keys[i];
                        newMover->pointers[i]->pointers[newMover->pointers[i]->keyTally+1] = newMover->pointers[i+1]->pointers[0];
                        newMover->pointers[i]->keyTally++;
                        newMover->keys[i] = newMover->pointers[i+1]->keys[0];
                        for (int j = 0; j < newMover->pointers[i+1]->keyTally-1; j++) {
                            newMover->pointers[i+1]->keys[j] = newMover->pointers[i+1]->keys[j+1];
                            newMover->pointers[i+1]->pointers[j] = newMover->pointers[i+1]->pointers[j+1];
                        }
                        newMover->pointers[i+1]->pointers[newMover->pointers[i+1]->keyTally-1] = newMover->pointers[i+1]->pointers[newMover->pointers[i+1]->keyTally];
                        newMover->pointers[i+1]->keyTally--;
                    }
                }
                newMover = findParent(newMover->keys[0]);
            }
        }
    }
    i = a;
    if (mover->pointers[i]->keyTally >= ceil(((double)M)/2 - 1)) {
        return;
    }
    if (i > 0 && (mover->pointers[i]->keyTally + mover->pointers[i-1]->keyTally + 1 <= M-1)) {
        BTreeMerge(mover, i-1);
        return;
    }
    if (i < mover->keyTally && (mover->pointers[i]->keyTally + mover->pointers[i+1]->keyTally + 1 <= M-1)){
        BTreeMerge(mover, i);
        return;
    }
    if (i > 0) {
        while(mover->pointers[i]->keyTally < ceil(((double)M)/2 - 1)) {
            mover->pointers[i]->keys[mover->pointers[i]->keyTally] = mover->pointers[i]->keys[mover->pointers[i]->keyTally-1];
            mover->pointers[i]->keys[mover->pointers[i]->keyTally-1] = mover->keys[i-1];
            mover->pointers[i]->keyTally++;
            for (int j = mover->pointers[i]->keyTally; j > 0; j--) {
                mover->pointers[i]->pointers[j] = mover->pointers[i]->pointers[j-1];
            }
            mover->keys[i-1] = mover->pointers[i-1]->keys[mover->pointers[i-1]->keyTally-1];
            mover->pointers[i]->pointers[0] = mover->pointers[i-1]->pointers[mover->pointers[i-1]->keyTally];
            mover->pointers[i-1]->pointers[mover->pointers[i-1]->keyTally] = nullptr;
            mover->pointers[i-1]->keyTally--;
        }
    }
    if (i < mover->keyTally) {
        while(mover->pointers[i]->keyTally < ceil(((double)M)/2 - 1)) {
            mover->pointers[i]->keys[mover->pointers[i]->keyTally] = mover->keys[i];
            mover->pointers[i]->pointers[mover->pointers[i]->keyTally+1] = mover->pointers[i+1]->pointers[0];
            mover->pointers[i]->keyTally++;
            mover->keys[i] = mover->pointers[i+1]->keys[0];
            for (int j = 0; j < mover->pointers[i+1]->keyTally-1; j++) {
                mover->pointers[i+1]->keys[j] = mover->pointers[i+1]->keys[j+1];
                mover->pointers[i+1]->pointers[j] = mover->pointers[i+1]->pointers[j+1];
            }
            mover->pointers[i+1]->pointers[mover->pointers[i+1]->keyTally-1] = mover->pointers[i+1]->pointers[mover->pointers[i+1]->keyTally];
            mover->pointers[i+1]->keyTally--;
        }
    }
}

template <typename T, int M>
void BTree<T, M>::traversal(BTreeNode<T, M>* &node) {
    for (int i=0;i < node->keyTally; i++) {
        if (!node->leaf) {
            traversal(node->pointers[i]);
        }
        std::cout << node->keys[i] << std::endl;
    }
    if (!node->leaf) {
        traversal(node->pointers[node->keyTally]);
    }
}

#endif /* btree_h */
