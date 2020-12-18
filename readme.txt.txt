The Reel
-------------------
A mini search system on a movie review dataset

Data set collection
----------------------

The data set is collected from two sources. The following are links to them.

1. http://ai.stanford.edu/~amaas/data/sentiment/

This is a data set used for sentiment analysis and is divided into training and test sets for positive, negative labels and unsupervised learning. I combined all the files into one data set and renamed sequentially using a python script. This data set consists of one lakh movie reviews.

2. https://md-datasets-cache-zipfiles-prod.s3.eu-west-1.amazonaws.com/38j8b6s2mx-1.zip

This data set contains 1000 moview reviews, 100 from 10 different movies each. I combined them into a single data set and renamed them using a python script.

The curated data set is too large to upload in the submission. Hence, it can be found at 

Basic Set up
------------------

This project has a simple set up. Entire code is in a single file named code.cpp. The code is written from scratch in C++ using standard template library. Hence, g++ has to be installed before to run this project.

The path to the data set has to be changed in BASEPATH definition at the top of the code before running.

Using the command line tool,

g++ code.cpp

We compile the code and

./a.exe (in Windows) or ./a.out (in Ubuntu/Linux).

This project is primarily developed on a windows system.

The output of indexing can be viewed at output.txt file.

Components/ Parts of the IR system
------------------------------------------

This project has three parts. They are:

1. Indexing (Implemented Positional Index)
2. Querying (Free text queries and One word queries)
3. Ranking (tf-idf weighting and cosine similarity scoring)

References
--------------

1. IR lecture slides
2. Introduction to Information Retrieval by Christopher Manning
3. http://www.ardendertat.com/2012/01/11/implementing-search-engines/

