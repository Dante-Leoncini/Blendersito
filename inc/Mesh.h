//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve };
typedef GLshort Edge[2];

typedef enum { pointLight, sunLight };

class Children {
	public:
		TInt Id;
		GLfixed OriginalScaleX;
		GLfixed OriginalScaleY; 
		GLfixed OriginalScaleZ;		
};

class Object {
	public:
		TInt type;
		TBool visible;
		TBool seleccionado;
		GLfloat posX; GLfloat posY; GLfloat posZ;
		GLfloat rotX; GLfloat rotY; GLfloat rotZ;
		GLfixed scaleX; GLfixed scaleY; GLfixed scaleZ;
		TInt Id;
		TInt Parent;
		RArray<Children> Childrens;
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
        RArray<TInt> edgeLinks;
		TBool seleccionado;
};

class NewVertexGroup { 
	public:
        TInt oldIndex;
        TInt newIndex;
};

class EdgesGroup { 
	public:
        TInt indicesA;
        TInt indicesB;
		RArray<TInt> faces;  // Indices de las caras que usan este borde
		TBool seleccionado;
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

		TInt SelectActivo;
		TInt SelectCount;
		TInt SelectEdgesCount;
		TInt SelectFacesCount;

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

		void Mesh::VertexSelect(TInt index, TBool select){
			vertexGroups[index].seleccionado = select;
			for(TInt i=0; i < vertexGroups[index].edgeLinks.Count(); i++){
				CheckEdgeSelect(vertexGroups[index].edgeLinks[i]);
			}
		}

		void Mesh::CheckEdgeSelect(TInt index){
			TBool seleccionA = vertexGroups[edgesGroups[index].indicesA].seleccionado;
			TBool seleccionB = vertexGroups[edgesGroups[index].indicesB].seleccionado;
			if (seleccionA && seleccionB && !edgesGroups[index].seleccionado){
				SelectEdgesCount++;
				edgesGroups[index].seleccionado = true;
			}
			else if (edgesGroups[index].seleccionado){
				edgesGroups[index].seleccionado = false;
				SelectEdgesCount--;
			}
		}
		
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

		void Mesh::NewVertexUI() {		
			delete[] vertexGroupUI; 
			delete[] vertexGroupUIcolor;		
			vertexGroupUI = new GLshort[vertexGroups.Count()*3];			
			vertexGroupUIcolor = new GLubyte[vertexGroups.Count()*4];
			UpdateVertexUI();
			UpdateVertexColorsUI();
		};

		void Mesh::RecalcularBordes(){
			for(int i=0; i < vertexGroups.Count(); i++){
				vertexGroups[i].edgeLinks.Close();
			}	

			edgesDrawnSize = edgesGroups.Count()*2;
			if (edges == NULL){delete[] edges;}
			edges = new GLushort[edgesDrawnSize];
			for(int a=0; a < edgesGroups.Count(); a++){
				edges[a*2] = edgesGroups[a].indicesA;
				edges[a*2+1] = edgesGroups[a].indicesB;
				//links de bordes
				vertexGroups[edgesGroups[a].indicesA].edgeLinks.Append(a);
				vertexGroups[edgesGroups[a].indicesB].edgeLinks.Append(a);
			}	
		}

		void Mesh::DuplicateVertices() {	
			TInt realSelectCount = 0;
			TInt vertexGroupsCount = vertexGroups.Count();
			for(int vg=0; vg < vertexGroupsCount; vg++){
				if ( vertexGroups[vg].seleccionado ){
					realSelectCount += vertexGroups[vg].indices.Count();
				}
			}

			//primero crea los array temporales y les suma el espacio del nuevo vertice
			GLshort* TempVertex = new GLshort[vertexSize*3+realSelectCount*3];
			GLbyte* TempNormals = new GLbyte[vertexSize*3+realSelectCount*3];
			GLubyte* TempColors = new GLubyte[vertexSize*4+realSelectCount*4];
			GLfloat* TempUv = new GLfloat[vertexSize*2+realSelectCount*2];
			
			vertexGroups.ReserveL(vertexGroupsCount+SelectCount);

			//copia los valores originales al array temporal
			for(int a=0; a < vertexSize*2; a++){
				TempUv[a] = uv[a];			
			}
			for(int a=0; a < vertexSize*3; a++){
				TempVertex[a] = vertex[a];
				TempNormals[a] = normals[a];	
			}
			for(int a=0; a < vertexSize*4; a++){
				TempColors[a] = vertexColor[a];	
			}

			//copia los vertices seleccionados	
			TInt newSelectActivo = 0;
			for(int vg=0; vg < vertexGroupsCount; vg++){
				if ( vertexGroups[vg].seleccionado ){
					vertexGroups[vg].seleccionado = false;
					VertexGroup TempVertexGroup;
					TInt indiceNewVG = vertexGroups.Count();
					TInt indicesCount = vertexGroups[vg].indices.Count();
					//si era el activo original	
					if (vg == SelectActivo){newSelectActivo = indiceNewVG;};		
					vertexGroups.Append(TempVertexGroup);
					vertexGroups[indiceNewVG].seleccionado = true;
					vertexGroups[indiceNewVG].indices.ReserveL(indicesCount);

					for(int i=0; i < indicesCount; i++){
						TInt NewIndice2 = vertexSize*2;
						TInt NewIndice3 = vertexSize*3;
						TInt NewIndice4 = vertexSize*4;
						TInt numeroVertice2 = vertexGroups[vg].indices[i]*2;
						TInt numeroVertice3 = vertexGroups[vg].indices[i]*3;
						TInt numeroVertice4 = vertexGroups[vg].indices[i]*4;

						TempUv[NewIndice2]   = uv[numeroVertice2];
						TempUv[NewIndice2+1] = uv[numeroVertice2+1];
				
						TempVertex[NewIndice3]   = vertex[numeroVertice3];
						TempVertex[NewIndice3+1] = vertex[numeroVertice3+1];	
						TempVertex[NewIndice3+2] = vertex[numeroVertice3+2];	

						TempNormals[NewIndice3]   = normals[numeroVertice3];
						TempNormals[NewIndice3+1] = normals[numeroVertice3+1];	
						TempNormals[NewIndice3+2] = normals[numeroVertice3+2];	

						TempColors[NewIndice4]   = vertexColor[numeroVertice4];
						TempColors[NewIndice4+1] = vertexColor[numeroVertice4+1];	
						TempColors[NewIndice4+2] = vertexColor[numeroVertice4+2];	
						TempColors[NewIndice4+3] = vertexColor[numeroVertice4+3];	

						vertexGroups[indiceNewVG].indices.Append(vertexSize);
						vertexSize++;
					}
				}		
			}
			SelectActivo = newSelectActivo;
			
			//libera memoria
			delete[] vertex;
			delete[] normals;
			delete[] vertexColor;
			//delete[] edges;
			delete[] uv;

			//los temporales ahora son los reales
			vertex = TempVertex;
			normals = TempNormals;
			vertexColor = TempColors;
			//edges = TempEdges;
			uv = TempUv;
			//borra la representacion grafica de la edicion y crea una nueva
			NewVertexUI();
		}
		
		void Mesh::ExtrudeVertices() {	
			TInt realSelectCount = 0;
			TInt vertexGroupsCount = vertexGroups.Count();
			for(int vg=0; vg < vertexGroupsCount; vg++){
				if ( vertexGroups[vg].seleccionado ){
					realSelectCount += vertexGroups[vg].indices.Count();
				}
			}

			//primero crea los array temporales y les suma el espacio del nuevo vertice
			GLshort* TempVertex = new GLshort[vertexSize*3+realSelectCount*3];
			GLbyte* TempNormals = new GLbyte[vertexSize*3+realSelectCount*3];
			GLubyte* TempColors = new GLubyte[vertexSize*4+realSelectCount*4];
			GLushort* TempEdges = new GLushort[edgesDrawnSize+SelectCount*2+SelectEdgesCount*2];
			GLfloat* TempUv = new GLfloat[vertexSize*2+realSelectCount*2];
			GLushort* TempFaces = new GLushort[facesCountIndices+SelectEdgesCount*6];
			
			TInt edgesGroupsCount = edgesGroups.Count();
			vertexGroups.ReserveL(vertexGroupsCount+SelectCount);
			edgesGroups.ReserveL(edgesGroupsCount+SelectCount+SelectEdgesCount);

			RArray<NewVertexGroup> newVertexGroup;
			newVertexGroup.ReserveL(SelectCount);

			//copia los valores originales al array temporal
			for(int a=0; a < vertexSize*2; a++){
				TempUv[a] = uv[a];			
			}
			for(int a=0; a < vertexSize*3; a++){
				TempVertex[a] = vertex[a];
				TempNormals[a] = normals[a];	
			}
			for(int a=0; a < vertexSize*4; a++){
				TempColors[a] = vertexColor[a];	
			}
			for(int a=0; a < edgesDrawnSize; a++){
				TempEdges[a] = edges[a];			
			}
			for(int a=0; a < facesCountIndices; a++){
				TempFaces[a] = faces[a];			
			}			

			//copia los vertices seleccionados	
			TInt newSelectActivo = 0;
			for(int vg=0; vg < vertexGroupsCount; vg++){
				if ( vertexGroups[vg].seleccionado ){
					vertexGroups[vg].seleccionado = false;
					VertexGroup TempVertexGroup;
					TInt indiceNewVG = vertexGroups.Count();
					TInt indicesCount = vertexGroups[vg].indices.Count();
					//si era el activo original	
					if (vg == SelectActivo){newSelectActivo = indiceNewVG;};		
					vertexGroups.Append(TempVertexGroup);
					vertexGroups[indiceNewVG].seleccionado = true;
					vertexGroups[indiceNewVG].indices.ReserveL(indicesCount);

					for(int i=0; i < indicesCount; i++){
						TInt NewIndice2 = vertexSize*2;
						TInt NewIndice3 = vertexSize*3;
						TInt NewIndice4 = vertexSize*4;
						TInt numeroVertice2 = vertexGroups[vg].indices[i]*2;
						TInt numeroVertice3 = vertexGroups[vg].indices[i]*3;
						TInt numeroVertice4 = vertexGroups[vg].indices[i]*4;

						TempUv[NewIndice2]   = uv[numeroVertice2];
						TempUv[NewIndice2+1] = uv[numeroVertice2+1];
				
						TempVertex[NewIndice3]   = vertex[numeroVertice3];
						TempVertex[NewIndice3+1] = vertex[numeroVertice3+1];	
						TempVertex[NewIndice3+2] = vertex[numeroVertice3+2];	

						TempNormals[NewIndice3]   = normals[numeroVertice3];
						TempNormals[NewIndice3+1] = normals[numeroVertice3+1];	
						TempNormals[NewIndice3+2] = normals[numeroVertice3+2];	

						TempColors[NewIndice4]   = vertexColor[numeroVertice4];
						TempColors[NewIndice4+1] = vertexColor[numeroVertice4+1];	
						TempColors[NewIndice4+2] = vertexColor[numeroVertice4+2];	
						TempColors[NewIndice4+3] = vertexColor[numeroVertice4+3];	

						vertexGroups[indiceNewVG].indices.Append(vertexSize);
						vertexSize++;
					}	
					// Duplicar bordes seleccionados
					EdgesGroup TempEdgesGroup;
					TInt indiceNewEG = edgesGroups.Count();
					edgesGroups.Append(TempEdgesGroup);
					edgesGroups[indiceNewEG].indicesA = vg;
					edgesGroups[indiceNewEG].indicesB = indiceNewVG;						
					vertexGroups[vg].edgeLinks.Append(indiceNewEG);		
					vertexGroups[indiceNewVG].edgeLinks.Append(indiceNewEG);								
					TempEdges[edgesDrawnSize]   = vg;
					TempEdges[edgesDrawnSize+1] = indiceNewVG;
					edgesDrawnSize+=2;

					NewVertexGroup TempNewVertexGroup;
					TempNewVertexGroup.oldIndex = vg;
					TempNewVertexGroup.newIndex = indiceNewVG;
					newVertexGroup.Append(TempNewVertexGroup);
				}		
			}
			//bordes seleccionados
			for (TInt eg = 0; eg < edgesGroupsCount; eg++) {
				if (edgesGroups[eg].seleccionado){
					TInt newIndexA = -1;
					TInt newIndexB = -1;
					//busca el indice donde estan guardado los indices viejos y nuevos
					for (TInt nv = 0; nv < newVertexGroup.Count(); nv++){
						//borde encontrado
						if (edgesGroups[eg].indicesA == newVertexGroup[nv].oldIndex){
							newIndexA = newVertexGroup[nv].newIndex;
						}
						else if (edgesGroups[eg].indicesB == newVertexGroup[nv].oldIndex){
							newIndexB = newVertexGroup[nv].newIndex;
						}
					}
					//detecta un error
					if (newIndexA < 0 || newIndexB < 0){
						continue;
					}
					
					EdgesGroup TempEdgesGroup;
					TInt indiceNewEG = edgesGroups.Count();
					edgesGroups.Append(TempEdgesGroup);
					edgesGroups[indiceNewEG].indicesA = newIndexA;
					edgesGroups[indiceNewEG].indicesB = newIndexB;		
									
					vertexGroups[newIndexA].edgeLinks.Append(indiceNewEG);		
					vertexGroups[newIndexB].edgeLinks.Append(indiceNewEG);								
					TempEdges[edgesDrawnSize]   = newIndexA;
					TempEdges[edgesDrawnSize+1] = newIndexB;
					edgesDrawnSize+=2;

					//nuevas caras
					facesCount += 2;
					TempFaces[facesCountIndices] = vertexGroups[edgesGroups[eg].indicesA].indices[0];
					TempFaces[facesCountIndices+1] = vertexGroups[newIndexA].indices[0];
					TempFaces[facesCountIndices+2] = vertexGroups[newIndexB].indices[0];
					facesCountIndices += 3;
					TempFaces[facesCountIndices] = vertexGroups[edgesGroups[eg].indicesB].indices[0];
					TempFaces[facesCountIndices+1] = vertexGroups[edgesGroups[eg].indicesA].indices[0];
					TempFaces[facesCountIndices+2] = vertexGroups[newIndexB].indices[0];
					facesCountIndices += 3;

					TInt indiceFG = facesGroup.Count()-1;
					facesGroup[indiceFG].count += 2; 
					facesGroup[indiceFG].indicesDrawnCount +=6;
				}
			}

			SelectActivo = newSelectActivo;
			
			//libera memoria
			newVertexGroup.Close();
			delete[] vertex;
			delete[] normals;
			delete[] vertexColor;
			if (edges != NULL){
				delete[] edges;
			}
			delete[] uv;
			delete[] faces;			

			//los temporales ahora son los reales
			vertex = TempVertex;
			normals = TempNormals;
			vertexColor = TempColors;
			edges = TempEdges;
			uv = TempUv;
			faces = TempFaces;
			//borra la representacion grafica de la edicion y crea una nueva
			NewVertexUI();
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
			TInt indiceReal = indice*4;
			vertexGroupUIcolor[indiceReal] = colorVerticeR;
			vertexGroupUIcolor[indiceReal+1] = colorVerticeG;
			vertexGroupUIcolor[indiceReal+2] = colorVerticeB;
			vertexGroupUIcolor[indiceReal+3] = 255;
		}

		void Mesh::UpdateVertexColorsUI(){	
			for(TInt vg=0; vg < vertexGroups.Count(); vg++){
				UpdateVertexColorUI(vg);
			}
		}

		void Mesh::AgruparVertices(){	
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
					if (vertexGroups.Count() > 0){
						newVertexGroups.seleccionado = false;
					}
					else {
						SelectActivo = 0;
						SelectCount = 1;
						newVertexGroups.seleccionado = true;
					}
					vertexGroups.Append(newVertexGroups);
					vertexGroups[vertexGroups.Count()-1].indices.Append(a);
				}
			}

			//ahora crea los vertices que openGL lee.			
			vertexGroupUI = new GLshort[vertexGroups.Count()*3];			
			vertexGroupUIcolor = new GLubyte[vertexGroups.Count()*4];
			UpdateVertexUI();
			UpdateVertexColorsUI();

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

			/*edges = new GLushort[edgesGroups.Count()*2];
			edgesDrawnSize = edgesGroups.Count()*2;
			for(TInt eg=0; eg < edgesGroups.Count(); eg++){
				edges[eg*2] = vertexGroups[edgesGroups[eg].indicesA[0]].indices[0];
				edges[eg*2+1] = vertexGroups[edgesGroups[eg].indicesB[0]].indices[0];
			};*/

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
