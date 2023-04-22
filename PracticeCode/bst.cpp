#include <iostream>

using namespace std;

class tree_node {
public:
  int _key;
  tree_node *lnode;
  tree_node *rnode;

public:
  tree_node(int key) {
    _key = key;
    lnode = NULL;
    rnode = NULL;
  }
};

int main(int argc, char *argv[])
{
  tree_node *root = new tree_node(10);

  root->lnode = new tree_node(20);
  root->rnode = new tree_node(30);
  root->lnode->lnode = new tree_node(40);

  return 0;
}