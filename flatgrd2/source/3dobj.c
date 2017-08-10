/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

OBJECT3D *load_object3d(char *filename)
{
	OBJECT3D *obj;
	FILE *p;

	if((p=fopen(filename,"r+b"))==NULL)
		return 0;

	if(getc(p)!='M' || fgetc(p)!='F' || fgetc(p)!='3' || fgetc(p)!='D')
	{
		fclose(p);
		return 0;
	}

	if((obj=(OBJECT3D *)malloc(sizeof(OBJECT3D)))==NULL)
	{
		fclose(p);
		return 0;
	}
	memset(obj,0,sizeof(OBJECT3D));

	fread(&obj->n_points,2,1,p);
	fread(&obj->n_faces,2,1,p);

	if(	(obj->points=(POINT3D *)malloc(obj->n_points*sizeof(POINT3D)))==NULL ||
		(obj->rotated_points=(POINT3D *)malloc(obj->n_points*sizeof(POINT3D)))==NULL ||
		(obj->translated_points=(POINT2D *)malloc(obj->n_points*sizeof(POINT2D)))==NULL ||
		(obj->point_normals=(VECTOR3D *)malloc(obj->n_points*sizeof(VECTOR3D)))==NULL ||
		(obj->rotated_point_normals=(VECTOR3D *)malloc(obj->n_points*sizeof(VECTOR3D)))==NULL ||
		(obj->faces=(FACE *)malloc(obj->n_faces*sizeof(FACE)))==NULL ||
		(obj->face_normals=(VECTOR3D *)malloc(obj->n_faces*sizeof(VECTOR3D)))==NULL ||
		(obj->face_depth=(short int *)malloc(obj->n_faces*sizeof(int)))==NULL ||
		(obj->order_table=(short int *)malloc(obj->n_faces*sizeof(int)))==NULL)

		//(obj->tex_coords = (tri_struct *)malloc(obj->n_faces * sizeof(tri_struct))) == NULL)
	{
		unload_object3d(obj);
		fclose(p);
		return 0;
	}

	fread(obj->points,sizeof(POINT3D),obj->n_points,p);
	fread(obj->faces,sizeof(FACE),obj->n_faces,p);

	init_face_normals(obj);
	init_point_normals(obj);

	//init_tex_coords(obj);

	obj->angle_x=0;
	obj->angle_y=0;
	obj->angle_z=0;

	fclose(p);
	return obj;
}

void unload_object3d(OBJECT3D *obj)
{
	if(obj->points) free(obj->points);
	if(obj->rotated_points) free(obj->rotated_points);
	if(obj->translated_points) free(obj->translated_points);
	if(obj->point_normals) free(obj->point_normals);
	if(obj->rotated_point_normals) free(obj->rotated_point_normals);
	if(obj->faces) free(obj->faces);
	if(obj->face_normals) free(obj->face_normals);
	if(obj->face_depth) free(obj->face_depth);
	if(obj->order_table) free(obj->order_table);

	if(obj->tex_coords) free(obj->tex_coords);

	if(obj) free(obj);
}

void init_face_normals(OBJECT3D *obj)
{
	int i;
	FACE *face;
	VECTOR3D *face_normal,v1,v2;
	POINT3D *p1,*p2,*p3;

	face=obj->faces;
	face_normal=obj->face_normals;

	for(i=0;i<obj->n_faces;i++)
	{
		p1=&obj->points[face->v1];
		p2=&obj->points[face->v2];
		p3=&obj->points[face->v3];

		v1.x=p2->x-p1->x;
		v1.y=p2->y-p1->y;
		v1.z=p2->z-p1->z;

		v2.x=p3->x-p2->x;
		v2.y=p3->y-p2->y;
		v2.z=p3->z-p2->z;

		cross_product(face_normal,&v1,&v2);
		normalize_vector(face_normal);

		face++;
		face_normal++;
	}
}

/*
void init_point_normals(OBJECT3D *obj)
{
	int i,j,max_hit_faces,hit_faces[8];
	VECTOR3D *point_normal,*face_normal;
	FACE *face;
	float x,y,z;

	point_normal=obj->point_normals;
	for(i=0;i<obj->n_points;i++)
	{
		max_hit_faces=0;
		face=obj->faces;

		for(j=0;j<obj->n_faces;j++)
		{
			if(face->v1==i || face->v2==i || face->v3==i)
				hit_faces[max_hit_faces++]=j;
		
			face++;
		}

		x=y=z=0;
		for(j=0;j<max_hit_faces;j++)
		{
			face_normal=&obj->face_normals[hit_faces[j]];
			x+=face_normal->x;
			y+=face_normal->y;
			z+=face_normal->z;
		}

		point_normal->x=x/max_hit_faces;
		point_normal->y=y/max_hit_faces;
		point_normal->z=z/max_hit_faces;
		normalize_vector(point_normal);

		point_normal++;
	}
}
*/

void init_point_normals(OBJECT3D *obj)
{
	int i,j,face_hit;
	float sum_x,sum_y,sum_z;
	VECTOR3D face_normal,v1,v2;
	POINT3D *p1,*p2,*p3;

	for(i=0;i<obj->n_points;i++)
	{
		sum_x=sum_y=sum_z=0;
		face_hit=0;

		for(j=0;j<obj->n_faces;j++)
		{
			if(obj->faces[j].v1==i || 
				obj->faces[j].v2==i || obj->faces[j].v3==i)
			{
				p1=&obj->points[obj->faces[j].v1];
				p2=&obj->points[obj->faces[j].v2];
				p3=&obj->points[obj->faces[j].v3];

				v1.x=p2->x-p1->x;
				v1.y=p2->y-p1->y;
				v1.z=p2->z-p1->z;

				v2.x=p3->x-p2->x;
				v2.y=p3->y-p2->y;
				v2.z=p3->z-p2->z;

				cross_product(&face_normal,&v1,&v2);

				sum_x+=face_normal.x;
				sum_y+=face_normal.y;
				sum_z+=face_normal.z;

				face_hit++;
			}
		}

		sum_x/=face_hit;
		sum_y/=face_hit;
		sum_z/=face_hit;

		v1.x=sum_x;
		v1.y=sum_y;
		v1.z=sum_z;

		normalize_vector(&v1);

		obj->point_normals[i].x=v1.x;
		obj->point_normals[i].y=v1.y;
		obj->point_normals[i].z=v1.z;
	}
}

void rotate_translate_object3d(OBJECT3D *obj,
			int delta_x,int delta_y,int delta_z)
{
	int i;

	obj->angle_x+=delta_x;
	obj->angle_y+=delta_y;
	obj->angle_z+=delta_z;

	mx_rotation_matrix(obj->object_matrix,
		obj->angle_x,obj->angle_y,obj->angle_z);

	/* rotate points */
	mx_rotate_points(obj->rotated_points,obj->points,
		obj->n_points,obj->object_matrix);

	/* rotate their normals */
	mx_rotate_points(obj->rotated_point_normals,
		obj->point_normals,obj->n_points,obj->object_matrix);

	for(i=0;i<obj->n_points;i++)
		translate_point(&obj->translated_points[i],&obj->rotated_points[i]);
}

void draw_object3d(OBJECT3D *obj,VECTOR3D *light,int drawing_mode)
{
	int i,j,x1,y1,x2,y2,x3,y3;
	FACE *face;
	POINT2D *point;
	VECTOR3D rotated_normal;
	unsigned char color,point_colors[3];
	float dot;
	short face_nr,*vertex;
	int tex_coords[3][2];
	tri_struct t,t_bump,t_env;

	sort_faces(obj);

	for(i=0;i<obj->visible_faces;i++)
	{
		face_nr=obj->order_table[obj->visible_faces-1-i];
		face=&obj->faces[face_nr];

		point=&obj->translated_points[face->v1];
		x1=point->x;
		y1=point->y;
		point=&obj->translated_points[face->v2];
		x2=point->x;
		y2=point->y;
		point=&obj->translated_points[face->v3];
		x3=point->x;
		y3=point->y;

		if(drawing_mode==FLAT_SHADING)
		{
			mx_rotate_points(&rotated_normal,&obj->face_normals[face_nr],1,
				obj->object_matrix);

			dot=dot_product(&rotated_normal,light);
						
			if(dot>1 || dot<0) dot=0;
			color=(unsigned char)(dot*MAX_PHONG_COLORS);

			flat_triangle(x1,y1,x2,y2,x3,y3,color,frame_buffer);
		}
		else if(drawing_mode==GOURAUD_SHADING)
		{
			vertex=&face->v1;
			for(j=0;j<3;j++,vertex++)
			{
				dot=dot_product(&obj->rotated_point_normals[*vertex],light);
				if(dot>1 || dot<0) dot=0;
				point_colors[j]=(unsigned char)(dot*MAX_PHONG_COLORS);
			}

			gouraud_triangle(x1,y1,x2,y2,x3,y3,point_colors[0],
					point_colors[1],point_colors[2],frame_buffer);
		}
		else if(drawing_mode == ENVIRONMENT_MAPPING || drawing_mode == ENV_BUMP)
		{
			vertex=&face->v1;
			for(j=0;j<3;j++,vertex++)
			{
				tex_coords[j][0]=(int)((obj->rotated_point_normals[*vertex].x*63)+63);
				tex_coords[j][1]=(int)((obj->rotated_point_normals[*vertex].y*63)+63);
			}

			if(drawing_mode == ENVIRONMENT_MAPPING)
				textured_triangle(x1,y1,x2,y2,x3,y3,
				tex_coords[0][0],tex_coords[0][1],
				tex_coords[1][0],tex_coords[1][1],
				tex_coords[2][0],tex_coords[2][1],envmap,frame_buffer);
			/*else
			{
				t.x1 = x1;
				t.y1 = y1;
				t.x2 = x2;
				t.y2 = y2;
				t.x3 = x3;
				t.y3 = y3;

				t_env.x1 = tex_coords[0][0];
				t_env.y1 = tex_coords[0][1];
				t_env.x2 = tex_coords[1][0];
				t_env.y2 = tex_coords[1][1];
				t_env.x3 = tex_coords[2][0];
				t_env.y3 = tex_coords[2][1];

				memcpy(&t_bump,&obj->tex_coords[face_nr],sizeof(tri_struct));

				bumpmapped_triangle(&t,&t_bump,&t_env,
					bumpmap,envmap,frame_buffer);
			}*/

		}
	}
}

/* z-sorting routines */

void sort_faces(OBJECT3D *obj)
{
	int i,j;
	FACE *face;
	POINT3D *vertex;
	float depth;
	int x1,y1,x2,y2,x3,y3;
	POINT2D *v2d;

	face=obj->faces;
	obj->visible_faces=0;

	for(i=0;i<obj->n_faces;i++,face++)
	{
		v2d=&obj->translated_points[face->v1];
		x1=v2d->x;
		y1=v2d->y;
		v2d=&obj->translated_points[face->v2];
		x2=v2d->x;
		y2=v2d->y;
		v2d=&obj->translated_points[face->v3];
		x3=v2d->x;
		y3=v2d->y;

		if(((y1-y3)*(x2-x1)-(x1-x3)*(y2-y1))<=0)		/* skip invisible face */
			continue;

		obj->order_table[obj->visible_faces]=i;

		vertex=&obj->rotated_points[face->v1];
		depth=vertex->z;
		vertex=&obj->rotated_points[face->v2];
		depth+=vertex->z;
		vertex=&obj->rotated_points[face->v3];
		depth+=vertex->z;

		obj->face_depth[obj->visible_faces]=(short int)(depth*64);
		obj->visible_faces++;
	}

	depth_sorting(obj,0,obj->visible_faces-1);
}

void depth_sorting(OBJECT3D *obj,short int left,short int right)
{
	short int i,j,tmp,element;

	i=left;
	j=right;
	element=obj->face_depth[(left+right)>>1];

	while(i<j)
	{
		while(obj->face_depth[i]<element) i++;
		while(obj->face_depth[j]>element) j--;

		if(i<=j)
		{
			tmp=obj->face_depth[i];
			obj->face_depth[i]=obj->face_depth[j];
			obj->face_depth[j]=tmp;

			tmp=obj->order_table[i];
			obj->order_table[i]=obj->order_table[j];
			obj->order_table[j]=tmp;

			i++;
			j--;
		}

	}

	if(j>left) depth_sorting(obj,left,j);
	if(i<right) depth_sorting(obj,i,right);
}

/*
void init_tex_coords(OBJECT3D *o)
{
	int i;
	int coord;

	coord = 0;
	for(i=0;i<o->n_faces;i++)
	{
		//planar mapping

		//o->tex_coords[i].x1 = o->point_normals[o->faces[i].v1].x*63 + 63;
		//o->tex_coords[i].y1 = o->point_normals[o->faces[i].v1].y*63 + 63;

		//o->tex_coords[i].x2 = o->point_normals[o->faces[i].v2].x*63 + 63;
		//o->tex_coords[i].y2 = o->point_normals[o->faces[i].v2].y*63 + 63;

		//o->tex_coords[i].x3 = o->point_normals[o->faces[i].v3].x*63 + 63;
		//o->tex_coords[i].y3 = o->point_normals[o->faces[i].v3].y*63 + 63;


		o->tex_coords[i].x1 = coord;
		o->tex_coords[i].y1 = coord;

		if(!(i&1))
		{
			o->tex_coords[i].x2 = coord + 45;
			o->tex_coords[i].y2 = coord;

			o->tex_coords[i].x3 = coord + 45;
			o->tex_coords[i].y3 = coord + 45;
		}
		else
		{
			o->tex_coords[i].x2 = coord + 45;
			o->tex_coords[i].y2 = coord + 45;

			o->tex_coords[i].x3 = coord;
			o->tex_coords[i].y3 = coord + 45;

			coord = (i * 45) & 127;
		}

	
	}
}
*/

/*
void save_tex_coords(OBJECT3D *o, char *filename)
{
	FILE *p;
	int i,j,*k;
	char txt_buffer[32];

	p = fopen(filename,"w");

	itoa(o->n_faces,txt_buffer,10);
	fprintf(p,"%s%s%s","N_FACES equ ",txt_buffer,"\n\ntex_coords:"); 
	
	for(i=0;i<o->n_faces;i++)
	{
		fprintf(p,"%s","\n\tdb ");

		k = &o->tex_coords[i].x1;
		for(j=0;j<6;j++, k++)
		{
			itoa(*k,txt_buffer,10);
			fprintf(p,"%s",txt_buffer);

			if(j!=5) fputc(',',p);
		}
	}

	fclose(p);
}*/