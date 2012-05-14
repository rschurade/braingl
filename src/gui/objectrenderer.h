/*
 * objectrenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

class DataStore;

class ObjectRenderer
{
public:
	ObjectRenderer( DataStore* dataStore );
	virtual ~ObjectRenderer();

	virtual void init() = 0;
	virtual void initShader() = 0;
	virtual void initGeometry() = 0;
	virtual void setShaderVars() = 0;
	//virtual void draw() = 0;

protected:
	DataStore* m_dataStore;
};

#endif /* OBJECTRENDERER_H_ */
