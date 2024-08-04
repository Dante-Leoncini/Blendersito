//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve };
typedef GLshort Edge[2];

typedef enum { pointLight, sunLight };

class Object {
	public:
		TInt type;
		TBool visible;
		TBool seleccionado;
		GLfloat posX; GLfloat posY; GLfloat posZ;
		GLfloat rotX; GLfloat rotY; GLfloat rotZ;
		GLfixed scaleX; GLfixed scaleY; GLfixed scaleZ;
		TInt Id;
		RArray<TInt> Childrens;
		HBufC* name;
};

class Light { 
	public:
		TInt type;
		TInt lightId;
		GLfloat color[4];
};

class Material { 
	public:
		TBool textura;
		TBool transparent;
		TBool vertexColor;
		TBool lighting;
		TBool repeat;
		TBool culling;
		TInt interpolacion;
		GLuint textureID;
		GLfloat diffuse[4];		
		GLfloat specular[4];	
		GLfloat emission[4];
		HBufC* name;
};

class VertexGroup { 
	public:
        RArray<GLushort> indices;
		TBool seleccionado;
};

class EdgesGroup { 
	public:
        RArray<GLushort> indicesA;
        RArray<GLushort> indicesB;
};

class FacesGroup { 
	public:
        TInt start; //donde esta el primer triangulo real
        TInt count; //cuantos triangulos son reales

        TInt startDrawn; //indice del primer triangulo para dibujar
		TInt indicesDrawnCount; //cuantos vertices son
		TInt material; //de que material
};

class Mesh { 
	public:
		TBool smooth;
		TInt vertexSize;
		GLshort* vertex;
		GLubyte* vertexColor;
		GLbyte* normals;
		GLfloat* uv;
   		GLushort* faces;
   		GLushort* edges;

		//grupo de caras
		TInt facesCount;
		TInt facesCountIndices;
        RArray<FacesGroup> facesGroup;

		//nuevo vertex group
		GLshort* vertexGroupUI;
		GLubyte* vertexGroupUIcolor;
        RArray<VertexGroup> vertexGroups;

		//nuevo bordes group
		TInt edgesDrawnSize;
        RArray<EdgesGroup> edgesGroups;

		void Mesh::VaciarGrupos() {
			if (vertexGroupUI != NULL){
				delete[] vertexGroupUI; 
				vertexGroupUI = NULL;
			}
			if (vertexGroupUIcolor != NULL){
				delete[] vertexGroupUIcolor; 
				vertexGroupUIcolor = NULL;
			}
			for(TInt i=0; i < vertexGroups.Count(); i++){
				vertexGroups[i].indices.Close();
			}
			vertexGroups.Close();
			//bordes			
			edgesDrawnSize = 0;
			if (edges != NULL){
				delete[] edges; 
				edges = NULL;
			}
			for(TInt i=0; i < edgesGroups.Count(); i++){
				edgesGroups[i].indicesA.Close();
				edgesGroups[i].indicesB.Close();
			}
			edgesGroups.Close();
		};
		
		void Mesh::UpdateVertexUI(TInt EditSelect){	
			TInt indiceVertex = vertexGroups[EditSelect].indices[0]*3;
			vertexGroupUI[EditSelect*3] = vertex[indiceVertex];
			vertexGroupUI[EditSelect*3+1] = vertex[indiceVertex+1];
			vertexGroupUI[EditSelect*3+2] = vertex[indiceVertex+2];
		}

		void Mesh::UpdateVertexUI(){	
			for(TInt s=0; s < vertexGroups.Count(); s++){
				TInt indiceVertex = vertexGroups[s].indices[0]*3;
				vertexGroupUI[s*3] = vertex[indiceVertex];
				vertexGroupUI[s*3+1] = vertex[indiceVertex+1];
				vertexGroupUI[s*3+2] = vertex[indiceVertex+2];
			}
		}

		void Mesh::UpdateVertexColorUI(TInt indice){	
			GLubyte colorVerticeR = 0;
			GLubyte colorVerticeG = 0;
			GLubyte colorVerticeB = 0;
			if (vertexGroups[indice].seleccionado){
				colorVerticeR = 240;
				colorVerticeG = 150;
				colorVerticeB = 43;
			}
			vertexGroupUIcolor[indice*4] = colorVerticeR;
			vertexGroupUIcolor[indice*4+1] = colorVerticeG;
			vertexGroupUIcolor[indice*4+2] = colorVerticeB;
			vertexGroupUIcolor[indice*4+3] = 255;
		}

		void Mesh::UpdateVertexColorsUI(){	
			for(TInt vg=0; vg < vertexGroups.Count(); vg++){
				UpdateVertexColorUI(vg);
			}
		}

		void Mesh::AgruparVertices(){	
			VaciarGrupos();
			for(TInt a=0; a < vertexSize; a++){
		    	TBool iguales = false;
				//busca copias
				for(TInt s=0; s < vertexGroups.Count(); s++){
					TInt indiceVertex = vertexGroups[s].indices[0];
					if (vertex[indiceVertex*3] == vertex[a*3] &&
						vertex[indiceVertex*3+1] == vertex[a*3+1] &&
						vertex[indiceVertex*3+2] == vertex[a*3+2]){
						iguales = true;
						vertexGroups[s].indices.Append(a);
						break;
					}
				}				
				//si no se encontro el vertice en ningun grupo. crea uno y lo guarda adentro
				if (!iguales){
					VertexGroup newVertexGroups;
					newVertexGroups.seleccionado = false;
					vertexGroups.Append(newVertexGroups);
					vertexGroups[vertexGroups.Count()-1].indices.Append(a);
				}
			}

			//ahora crea los vertices que openGL lee.			
			vertexGroupUI = new GLshort[vertexGroups.Count()*3];			
			vertexGroupUIcolor = new GLubyte[vertexGroups.Count()*4];
			UpdateVertexUI();

			//agrupar bordes
			/*EdgesGroup nuevoEdgesGroup;	
			for(TInt fg=0; fg < facesGroup.Count(); fg++){
				//las caras tienen 3 bordes. revisa cada borde
				//for(TInt f=facesGroup[fg].start; f < facesGroup[fg].start + facesGroup[fg].count; f++){
				edgesGroups.Append(nuevoEdgesGroup);
				edgesGroups[edgesGroups.Count()-1].indicesA.Append(facesGroup[fg].start*3);
				edgesGroups[edgesGroups.Count()-1].indicesB.Append(facesGroup[fg].start*3+1);
				
				edgesGroups.Append(nuevoEdgesGroup);
				edgesGroups[edgesGroups.Count()-1].indicesA.Append(facesGroup[fg].start*3+1);
				edgesGroups[edgesGroups.Count()-1].indicesB.Append(facesGroup[fg].start*3+2);

				edgesGroups.Append(nuevoEdgesGroup);
				edgesGroups[edgesGroups.Count()-1].indicesA.Append(facesGroup[fg].start*3+2);
				edgesGroups[edgesGroups.Count()-1].indicesB.Append(facesGroup[fg].start*3);
				//}
			}	*/

			/*edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(0);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(1);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(1);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(2);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(2);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(3);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(3);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(0);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(4);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(5);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(5);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(6);

			edgesGroups.Append(nuevoEdgesGroup);
			edgesGroups[edgesGroups.Count()-1].indicesA.Append(6);
			edgesGroups[edgesGroups.Count()-1].indicesB.Append(7);*/

			edges = new GLushort[edgesGroups.Count()*2];
			edgesDrawnSize = edgesGroups.Count()*2;
			for(TInt eg=0; eg < edgesGroups.Count(); eg++){
				edges[eg*2] = vertexGroups[edgesGroups[eg].indicesA[0]].indices[0];
				edges[eg*2+1] = vertexGroups[edgesGroups[eg].indicesB[0]].indices[0];
			};
				//}
			//}
			/*class EdgesGroup { 
				public:
					RArray<GLushort> indices;
			};
			
			class FacesGroup { 
				public:
					TInt start; //donde esta el primer triangulo
					TInt count; //cuantos triangulos son
					TInt indicesCount; //cuantos vertices son
					TInt material;
			};*/
			
			/*for(TInt m=0; m < materialsSize; m++){
				for(TInt f=0; f < facesGroupsSize[m]; f++){
					(faces[m][f*3], faces[m][f*3+1])
					(faces[m][f*3+1], faces[m][f*3+2])
					(faces[m][f*3], faces[m][f*3+2])
				}
			};*/

			//agrupar vertices
		    /*GLshort* TempVertexPos = new GLshort[vertexSize];
		    GLushort* TempVertexIndice = new GLushort[vertexSize];
		    TInt* TempVertexIndiceGroupSize = new TInt[vertexSize];
		    GLushort** TempVertexIndiceGroup = new GLushort*[vertexSize];
			for(TInt a=0; a < vertexSize; a++){
				TempVertexPos[a] = 0;//obj.vertex[a];
				//TempVertexIndiceGroup[a] = new GLushort[obj.vertexSize/3];
				TempVertexIndiceGroup[a] = new GLushort[5]; //lo ideal seria el maximo posible. pero problemas de memoria
			}	
		    vertexGroupSize = 0;
		    TBool iguales = false;
			for(TInt a=0; a < vertexSize/3; a++){
				iguales = false;
				//busca copias
				for(TInt s=0; s < vertexGroupSize; s++){
					if (TempVertexPos[s*3] == vertex[a*3] &&
						TempVertexPos[s*3+1] == vertex[a*3+1] &&
						TempVertexPos[s*3+2] == vertex[a*3+2]){
						iguales = true;
						//aumenta el tama�o del array y le agrega el indice que coincide
						//GLushort* temp = new GLushort[TempVertexIndiceGroupSize[s]+1];
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	temp[t] = TempVertexIndiceGroup[s][t];			
						//}	
						//temp[TempVertexIndiceGroupSize[s]] = a;	
						//TempVertexIndiceGroupSize[s]++;
						//TempVertexIndiceGroup[s] = new GLushort[TempVertexIndiceGroupSize[s]]; //agrega el primer indice
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	TempVertexIndiceGroup[s][t] = temp[t];			
						//}
						//delete[] temp;
						TempVertexIndiceGroup[s][TempVertexIndiceGroupSize[s]] = a;
						TempVertexIndiceGroupSize[s]++;
						break;
					}			
				};
				//si no se encontro el vertice. lo guarda y aumenta el size
				if (!iguales){
					TempVertexPos[vertexGroupSize*3] = vertex[a*3];
					TempVertexPos[vertexGroupSize*3+1] = vertex[a*3+1];
					TempVertexPos[vertexGroupSize*3+2] = vertex[a*3+2];
					TempVertexIndice[vertexGroupSize] = a;	
					
					//TempVertexIndiceGroup[obj.vertexGroupSize] = new GLushort[1]; //agrega el primer indice
					TempVertexIndiceGroupSize[vertexGroupSize] = 1; //arranca en uno
					TempVertexIndiceGroup[vertexGroupSize][0] = a;
					vertexGroupSize++;
				}
			}
			//copia los indices de los vertices a dibujar
			vertexGroup = new GLushort[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroup[a] = TempVertexIndice[a];
			}	
			vertexGroupIndice = new GLushort*[vertexGroupSize];
			vertexGroupIndiceSize = new TInt[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroupIndiceSize[a] = TempVertexIndiceGroupSize[a];
				vertexGroupIndice[a] = new GLushort[vertexGroupIndiceSize[a]];
				for(TInt i=0; i < vertexGroupIndiceSize[a]; i++){
					vertexGroupIndice[a][i] = TempVertexIndiceGroup[a][i];	
				}
			}	

			// Liberar memoria de TempVertexPos, TempVertexIndice y TempVertexIndiceGroup
			delete[] TempVertexPos;
			delete[] TempVertexIndice;

			// Liberar memoria de TempVertexIndiceGroup y sus elementos
			for (TInt k = 0; k < vertexSize; ++k) {
				delete[] TempVertexIndiceGroup[k];
			}
			delete[] TempVertexIndiceGroup;*/
		};
};

/*
class Mesh { //clase Mesh
	public:	
		GLushort* vertexGroup;
		TInt* vertexGroupIndiceSize;
		GLushort** vertexGroupIndice;
		
		RArray<GLushort> NewFaces;	
		
		void Mesh::RemoveVertex(TInt vertexToRemove){
			vertexGroupSize--;
			if (vertexGroupSize <= 0){
				vertexSize = 0;
				normalsSize = 0;
				facesSize = 0;
				edgesSize = 0;
				uvSize = 0;
				vertexGroup = new GLushort[0];
				vertexGroupIndice = new GLushort*[0];
				vertexGroupIndiceSize = new TInt[0];
				vertex = new GLshort[0];
				normals = new GLbyte[0];
				return;
			}
			GLshort * newVertex = new GLshort[vertexSize];
			GLbyte * newNormals = new GLbyte[vertexSize];
			GLbyte* newUv = new GLbyte[uvSize];
			GLushort* newVertexGroup = new GLushort[vertexGroupSize];
			GLushort** newVertexGroupIndice = new GLushort*[vertexGroupSize];
			TInt* newVertexGroupIndiceSize = new TInt[vertexGroupSize];
			TInt newSize = 0;
			//solo guarda los vertices que no estan en el grupo
			TBool encontrado = false;
			for(TInt i = 0; i < vertexSize/3; i++){
				encontrado = false;
				for(TInt e = 0; e < vertexGroupIndiceSize[vertexToRemove]; e++){
					//si coincide el vertice con el indice del grupo a borrar, no lo copia
					if (vertexGroupIndice[vertexToRemove][e] == i){
						encontrado = true;						
					}
				}	
				if (!encontrado){
					newVertex[newSize*3] = vertex[i*3];
					newVertex[newSize*3+1] = vertex[i*3+1];
					newVertex[newSize*3+2] = vertex[i*3+2];
					newNormals[newSize*3] = normals[i*3];
					newNormals[newSize*3+1] = normals[i*3+1];
					newNormals[newSize*3+2] = normals[i*3+2];
					newUv[newSize*2] = uv[i*2];
					newUv[newSize*2+1] = uv[i*2+1];
					newSize++;								
				}	
			}
			//borra 3 por cada vertice en el grupo
			vertexSize -= vertexGroupIndiceSize[vertexToRemove]*3;
			uvSize -= vertexGroupIndiceSize[vertexToRemove]*2;
			normalsSize = vertexSize;
			newSize = 0;	
			TInt Restar = 0;
			for(TInt i = 0; i < vertexGroupSize+1; i++){
				if (i != vertexToRemove) {
					newVertexGroup[newSize] = vertexGroup[i];
					Restar = 0; //reinicia
					for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
						if (vertexGroupIndice[vertexToRemove][g] <= newVertexGroup[newSize]){Restar++;}					
					}
					newVertexGroup[newSize] -= Restar;
					newVertexGroupIndiceSize[newSize] = vertexGroupIndiceSize[i];
					newVertexGroupIndice[newSize] = new GLushort[vertexGroupIndiceSize[i]];
					for(TInt s = 0; s < vertexGroupIndiceSize[i]; s++){
						newVertexGroupIndice[newSize][s] = vertexGroupIndice[i][s];	
						Restar = 0; //reinicia
						for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
							if (vertexGroupIndice[vertexToRemove][g] <= newVertexGroupIndice[newSize][s]){
								Restar++;						
							}					
						}
						newVertexGroupIndice[newSize][s] -= Restar;
					}
					newSize++;
				}				
			}	
			//esto hay que ahcerlo ANTES de borrar los viejos datos
			//tenemos que cambiar los indices de los Array temporales
			vertexGroup = new GLushort[vertexGroupSize - 1];
			vertexGroupIndice = new GLushort*[vertexGroupSize - 1];
			vertexGroupIndiceSize = new TInt[vertexGroupSize - 1];
			vertex = new GLshort[vertexSize];
			normals = new GLbyte[vertexSize];
			uv = new GLbyte[uvSize];
			for(TInt i = 0; i < vertexSize; i++) {
				vertex[i] = newVertex[i];	
				normals[i] = newNormals[i];	
			}
			for(TInt i = 0; i < uvSize; i++) {
				uv[i] = newUv[i];	
			}
			for(TInt i = 0; i < vertexGroupSize; i++) {
				vertexGroup[i] = newVertexGroup[i];
				vertexGroupIndiceSize[i] = newVertexGroupIndiceSize[i];
				vertexGroupIndice[i] = new GLushort[newVertexGroupIndiceSize[i]];
				for(TInt s = 0; s < newVertexGroupIndiceSize[i]; s++){
					vertexGroupIndice[i][s] = newVertexGroupIndice[i][s];						
				}
			}		
	
			delete[] newVertex;
			delete[] newNormals;
			delete[] newUv;	
			delete[] newVertexGroup;
			delete[] newVertexGroupIndiceSize;
			for (TInt k = 0; k < vertexGroupSize; ++k) {
				delete[] newVertexGroupIndice[k];
			}
			delete[] newVertexGroupIndice;
		}

		void Mesh::RemoveFaces(RArray<GLushort>& facesIndexToRemove, TInt vertexToRemove) {
			GLushort * newFaces = new GLushort[facesSize - facesIndexToRemove.Count()*3];
			TInt newSize = 0;
			for(TInt i = 0; i < facesSize; i += 3) {
				TInt j = 0;
				for (; j < facesIndexToRemove.Count(); j++) {
					if (i/3 == facesIndexToRemove[j]) break;
				}
				if (j == facesIndexToRemove.Count()) {
					newFaces[newSize] = faces[i];
					newFaces[newSize + 1] = faces[i + 1];
					newFaces[newSize + 2] = faces[i + 2];
					newSize += 3;
				}
			}
			facesSize = newSize;
			delete[] faces;
			faces = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				faces[i] = newFaces[i];
				TInt Restar = 0;
				//resta un indice cada vez que un valor es superior al indice del grupo
				for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
					if (vertexGroupIndice[vertexToRemove][g] <= faces[i]){
						Restar++;						
					}					
				}
				faces[i] -= Restar;
			}
			delete[] newFaces;
		}
		void Mesh::RemoveEdges(RArray<GLushort>& edgesIndexToRemove, TInt vertexToRemove) {
		    GLushort * newEdges = new GLushort[edgesSize - edgesIndexToRemove.Count()*2];
		    TInt newSize = 0;
		    for(TInt i = 0; i < edgesSize; i += 2) {
		        TInt j = 0;
		        for (; j < edgesIndexToRemove.Count(); j++) {
		            if (i/2 == edgesIndexToRemove[j]) break;
		        }
		        if (j == edgesIndexToRemove.Count()) {
		            newEdges[newSize] = edges[i];
		            newEdges[newSize + 1] = edges[i + 1];
		            newSize += 2;
		        }
		    }
		    edgesSize = newSize;
		    edges = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				edges[i] = newEdges[i];
				TInt Restar = 0;
				//resta un indice cada vez que un valor es superior al indice del grupo
				for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
					if (vertexGroupIndice[vertexToRemove][g] <= edges[i]){
						Restar++;						
					}					
				}
				edges[i] -= Restar;
			}
		    delete[] newEdges;
		}
		void Mesh::AgruparVertices() {
			//agrupar vertices
		    GLshort* TempVertexPos = new GLshort[vertexSize];
		    GLushort* TempVertexIndice = new GLushort[vertexSize];
		    TInt* TempVertexIndiceGroupSize = new TInt[vertexSize];
		    GLushort** TempVertexIndiceGroup = new GLushort*[vertexSize];
			for(TInt a=0; a < vertexSize; a++){
				TempVertexPos[a] = 0;//obj.vertex[a];
				//TempVertexIndiceGroup[a] = new GLushort[obj.vertexSize/3];
				TempVertexIndiceGroup[a] = new GLushort[5]; //lo ideal seria el maximo posible. pero problemas de memoria
			}	
		    vertexGroupSize = 0;
		    TBool iguales = false;
			for(TInt a=0; a < vertexSize/3; a++){
				iguales = false;
				//busca copias
				for(TInt s=0; s < vertexGroupSize; s++){
					if (TempVertexPos[s*3] == vertex[a*3] &&
						TempVertexPos[s*3+1] == vertex[a*3+1] &&
						TempVertexPos[s*3+2] == vertex[a*3+2]){
						iguales = true;
						//aumenta el tama�o del array y le agrega el indice que coincide
						//GLushort* temp = new GLushort[TempVertexIndiceGroupSize[s]+1];
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	temp[t] = TempVertexIndiceGroup[s][t];			
						//}	
						//temp[TempVertexIndiceGroupSize[s]] = a;	
						//TempVertexIndiceGroupSize[s]++;
						//TempVertexIndiceGroup[s] = new GLushort[TempVertexIndiceGroupSize[s]]; //agrega el primer indice
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	TempVertexIndiceGroup[s][t] = temp[t];			
						//}
						//delete[] temp;
						TempVertexIndiceGroup[s][TempVertexIndiceGroupSize[s]] = a;
						TempVertexIndiceGroupSize[s]++;
						break;
					}			
				};
				//si no se encontro el vertice. lo guarda y aumenta el size
				if (!iguales){
					TempVertexPos[vertexGroupSize*3] = vertex[a*3];
					TempVertexPos[vertexGroupSize*3+1] = vertex[a*3+1];
					TempVertexPos[vertexGroupSize*3+2] = vertex[a*3+2];
					TempVertexIndice[vertexGroupSize] = a;	
					
					//TempVertexIndiceGroup[obj.vertexGroupSize] = new GLushort[1]; //agrega el primer indice
					TempVertexIndiceGroupSize[vertexGroupSize] = 1; //arranca en uno
					TempVertexIndiceGroup[vertexGroupSize][0] = a;
					vertexGroupSize++;
				}
			}
			//copia los indices de los vertices a dibujar
			vertexGroup = new GLushort[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroup[a] = TempVertexIndice[a];
			}	
			vertexGroupIndice = new GLushort*[vertexGroupSize];
			vertexGroupIndiceSize = new TInt[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroupIndiceSize[a] = TempVertexIndiceGroupSize[a];
				vertexGroupIndice[a] = new GLushort[vertexGroupIndiceSize[a]];
				for(TInt i=0; i < vertexGroupIndiceSize[a]; i++){
					vertexGroupIndice[a][i] = TempVertexIndiceGroup[a][i];	
				}
			}	

			// Liberar memoria de TempVertexPos, TempVertexIndice y TempVertexIndiceGroup
			delete[] TempVertexPos;
			delete[] TempVertexIndice;

			// Liberar memoria de TempVertexIndiceGroup y sus elementos
			for (TInt k = 0; k < vertexSize; ++k) {
				delete[] TempVertexIndiceGroup[k];
			}
			delete[] TempVertexIndiceGroup;
		}
		
		TBool Mesh::isDuplicateEdge(Edge* edges, TInt edgesSize, GLushort v1, GLushort v2) {
		    for (TInt i = 0; i < edgesSize; i++) {
		        if ((edges[i][0] == v1 && edges[i][1] == v2) || (edges[i][0] == v2 && edges[i][1] == v1)) {
		            return true;
		        }
		    }
		    return false;
		}

		void Mesh::RecalcularBordes() {
			for (int i = 0; i < facesSize/3; i += 3) {
				edges[i*6+0] = faces[i];   edges[i*6+1] = faces[i+1];
				edges[i*6+2] = faces[i+1]; edges[i*6+3] = faces[i+2];
				edges[i*6+4] = faces[i+2]; edges[i*6+5] = faces[i];
			}
			Edge* TempEdgesVertex = new Edge[edgesSize];
			TInt uniqueEdgeCount = 0;*/
			/*Edge edge1 = { 0, 0 };
			Edge edge2 = { 0, 0 };
			Edge edge3 = { 0, 0 };
			for (int i = 0; i < facesSize/3; i += 3) {
			    edge1[0] = faces[i];   edge1[1] = faces[i+1];
			    edge2[0] = faces[i+1]; edge2[1] = faces[i+2];
			    edge3[0] = faces[i+2]; edge3[1] = faces[i];
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge1[0], edge1[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge1[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge1[1];
				}
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge2[0], edge2[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge2[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge2[1];
				}
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge3[0], edge3[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge3[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge3[1];
				}
			}
			edgesSize = uniqueEdgeCount*2;
			for (int i = 0; i < edgesSize/2; i += 3) {
				edges[i*2] = TempEdgesVertex[i][0];
				edges[i*2+1] = TempEdgesVertex[i][1];
			}
			delete edge1;
			delete edge2;
			delete edge3;*/
			/*delete[] TempEdgesVertex;	
		}
};*/
