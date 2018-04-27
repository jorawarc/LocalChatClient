/*
 *
 * Node.h
 * 
 * Author: Jorawar Chana
 * Data Last Modified: Jan. 15, 2018
 *
 */

#ifndef NODE_H
#define NODE_H


typedef struct _node {
	struct _node * next;
	struct _node * last;
	void* data;
} Node;

#endif



