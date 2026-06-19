# Assignment 4: Answers

**Complete this document, commit your changes to Github and submit the repository URL to Canvas.** Keep your answers short and precise.

Names of all team members:
Brecken Lawrence
Matthew Virginia
Harrison Stein

Used free extension: [ ] 24 hrs or [x] 48 hrs

[ ] Early submission (48 hrs)

[ ] Bonus work. Describe: ...

Place [x] for what applies.


## Answers

### Project Management

Complete the tasks and planned completion columns **BEFORE** You start with 
coding!


Weekly milestones/tasks. Set out a plan at the beginning and then record when you got it done.

| Task        | Planned completion | Actual completion | Time spent in hrs by X | Time spent in hrs by Y |
| ----------- | :-----------------:| :---------------: | :--------------------: | 
:--------------------: |
| x           | Nov 3              | Nov 6             | 8.5                    | 0                      |

### High-level Design of Solution

- Add a UML class diagram, etc.
- High-level pseudo code for each component.
Check the GitHub repository for UML class diagram and a detailed description of what each function does.

### Used Data Structures
Explanation of what data structures are used where. For each, state
    - time and space complexity (Big-Oh),
    - why it is the appropriate data structure and what other data structure could have been used.

AVLTree(AVLOrganizer, AVL Child Classes, and AVLTree.h): Insertion, deletion, and search operations in AVL trees have an average time complexity of O(log n) where n is the number of nodes in the tree. The space complexity is O(n) where n is the number of nodes in the tree. This is an appropriate data structure as they are balanced binary search trees that maintain a logarithmic height, which is suitable for efficient searching and indexing. Red-Black tree could have been used as it offers balanced search tree characteristics and can be used for maintaining sorted search results or managing cached data.

std::unordered_set and std::unordered_map: For insertion, deletion and search the time complexity is O(1) on average and O(N) in worst-case. The space complexity is O(N). Unordered sets and unordered maps are appropriate in the search engine program as they are used for storing unique elements (set) and key value pairs (map) in the search engine. Alternatives could be std::set and std::map which provide ordered storage with logarithmic time complexities.

std::list: The time complexity for list with insertion and deletion is O(1) and with search it is O(N). The space complexity is O(N). A list is appropriate to use for the search engine program as it is used to store query search results, which results in quick insertion and deletion operations. An alternative would be a std::vector, for better access time.

### User Documentation
- What are the features of your software?
Index creation, index storage (loading and saving), querying, user interface, command line operations, and error handling for the search engine as a whole. Supersearch also works for command line operations in terminal when using the main.cpp for the user interface.

- Describe how to use the software.
The user interface of the software is handled by printing to the terminal and receiving input from the terminal that is associated with assigned argv values that correspond to menu options. Just type in your menu option like create_index and the program will create an index from the createIndex function in SearchEngine. In fact, all functions in the user interface correspond with the SearchEngine class as that is how the software is used for processing. For example, type in g++ main.cpp SearchEngine.cpp -o supersearch for compilation and type in supersearch create_index <directory> to create an index, supersearch save_index <directory> to save an index, supersearch load_index <directory> to load an index, and supersearch query <search terms> to perform a search query. These functions are also tested in the Parser_Test.cpp so you don't need to type these commands manually into terminal.

- Some example queries with the results.
For testDirectory from directory path "sample_data/coll_1":
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
Index created successfully.
Index created successfully.
Index saved to file: temp_direct/index_file.txt
Index loaded from file: temp_direct/index_file.txt

[Done] exited with code=3221225725 in 5.841 seconds

[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
Index created successfully.
Index created successfully.
Index saved to file: temp_direct/index_file.txt
Index loaded from file: temp_direct/index_file.txt

[Done] exited with code=3221225725 in 5.58 seconds

### Performance
- Provide statistics (word counts, timing, etc) for indexing all documents (or as many as you can in a reasonable amount of time).
We only tested the sample_data that we included with this implementation which included coll_1 and coll_2 where they had 3 .json files each. Took around 5.841 seconds to compile to index involving creation, saving to file, and loading from file. Each .json file had around 224 words. 

### Bonus Work
Did you attempt any bonuses? If so, please give a brief description of what you did.

   > None.
