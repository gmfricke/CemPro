compile=g++ -c -g
link=g++ -g

ifeq ($(shell uname),Darwin)
	libraries = -framework OpenGL -framework GLUT
else
	libraries = -lglut
endif

objects=driver.o Node.o List.o Protein.o CD4.o GP120.o CCR5.o Cluster.o Site.o Vertex.o Rectangle.o World.o Entity.o

cempro: 	driver.o
		$(link) -o cempro $(objects) $(libraries)

Node.o:		Node.cpp Node.h
		$(compile) Node.cpp

List.o:		List.cpp List.h Node.o
		$(compile) List.cpp

Protein.o:	Protein.cpp Protein.h Cluster.o
		$(compile) Protein.cpp

Site.o:		Site.cpp Site.h 
		$(compile) Site.cpp

World.o:	Site.o Protein.o CD4.o CCR5.o GP120.o Entity.o Rectangle.o List.o World.cpp World.h
		$(compile) World.cpp

Rectangle.o:	Rectangle.cpp Rectangle.h Vertex.o
		$(compile) Rectangle.cpp

Vertex.o:	Vertex.cpp Vertex.h
		$(compile) Vertex.cpp

Entity.o:	Entity.cpp Entity.h
		$(compile) Entity.cpp

Cluster.o:	Cluster.cpp Cluster.h
		$(compile) Cluster.cpp

driver.o:	World.o driver.cpp
		$(compile) driver.cpp

GP120.o:	GP120.cpp GP120.h Protein.o
		$(compile) GP120.cpp

CCR5.o:		CCR5.cpp CCR5.h Protein.o
		$(compile) CCR5.cpp

CD4.o:		CD4.cpp CD4.h Protein.o
		$(compile) CD4.cpp

clean:	
	rm *.o driver

