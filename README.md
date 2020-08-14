# simple-database
This is a simple database written by C, referenced from https://cstack.github.io/db_tutorial/

# Thoughts during coding:
1 Starting from a simple point. In this example, it starts from the simple UI, with an infinit loop to ask for input, until '.exit' is input.

2 Object orientation program is much easier, with encapsulation, inheritance, and polymorphism. Althougth C is not an OOL (language), several ways could make coding more clear, like trying to encapsulate data in structure like a class in C++, etc. In this case, several structures are used to encap data for each case, and build functions for each structure like setting and closing. It might be easier to be done in C++ or Java, and I might try C++ version if possible.

3 A big picture is necessary before coding. It makes things easier with a route map to guide code before coding, or coders might lose their mind during huge code. To achieve this, coders need to have a clear idea for what they need to. In this case, structure of database is the big picture.

4 Git is helpful for coding!

5 Comments are important for reviewing. Necessary and clear comments is easier for reviewing than less.

6 Naming should be meanful and clear. For example, function name 'execute_select' is better than 'select', especially if future extension with other 'select' related function will be added. 

7 State code in enum type is easier to be understood and managed than just a number, like 'META_COMMAND_SUCCESS' vs '0' in MetaCommandResult.

8 Remember to free memory before exiting.

9 Breaking the whole project into small parts, and complete each part at one time. This is easier for debug for each part. In this case, 'select' and 'insert' are two operations in the database, with simple UI in main function. So the idea is build the UI first, then add 'opertions' in related location, and write the real operations finally.

# Thoughts during debuging:
1 Debug is necessary and helpful. Try simple case first, and then edge case.

2 Auto test?
