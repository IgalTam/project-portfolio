import sys
import os
from random import randrange

def test_write(node_count: int):

    exst_count = len([name for name in os.listdir('./it_test_dir')]) + 1    # make unique file

    with open(f'./it_test_dir/it_test{exst_count}', 'w') as f:
        f.write(node_count + '\n')  # write n value
        node_count = int(node_count)
        for i in range(node_count): # create at least one edge for each vertex
            f.write(f"{i}, {randrange(0, node_count)}\n")
        for i in range(randrange(node_count)): # create a random amount of other edges between vertices
            f.write(f"{randrange(0, node_count)}, {randrange(0, node_count)}\n")

if __name__ == "__main__":
    test_write(sys.argv[1])