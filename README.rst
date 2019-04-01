
###############################
Python C\+\+ extension template
###############################

**********************************************
A template for a Python C\-API based extension
**********************************************
| **Why** would you want to have a **Python C\+\+\-extension** in the first place\?
| The answer is quite simple\: **Performance**\.
| Although you can achieve improved performance using `Cython <http://cython.org/>`_\, you can never beat **C\/C\+\+ level performance**\.
| 

About this template
===================

Introduction
------------
| This template provides a very basic Python extension written in C\/C\+\+\.
| The extension module \"*template*\" contains a function \"*test*\" and a class \"*example\_class*\"\.
| The \"*example\_class*\" has a single member \"*value*\" \(of type double \=\> float\)\,  and supports a lot of operations\.
| It also shows how to import a module \(\"math\.pi\" in this case\)\.
| 
| *Work in progress*\. 