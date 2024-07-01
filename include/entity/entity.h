#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"


/*
* Load a entity in the engine
* @filename: the path to the mesh (.obj) & texture (.png) for the entity
* @scaling: scaler for the entity
* @position: initial position
* @retation: initial rotation
*/
void load_entity(char* filename, vec3_t scaling, vec3_t position, vec3_t rotation);


/*
* Load a prop in the engine
* @filename: the path to the mesh (.obj) & texture (.png) for the entity
* @scaling: scaler for the entity
* @position: initial position
* @retation: initial rotation
*/
void load_prop(char* filename, vec3_t scaling, vec3_t position, vec3_t rotation);

#endif // !ENTITY_H
