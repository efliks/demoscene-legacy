struct3d struc
	s3d_n_points	dd ?			; number of points
	s3d_n_faces	dd ?			; number of faces
	s3d_vis_faces	dd ?			; number of visible faces

	s3d_points	dd ?			; base points
	s3d_r_points	dd ?			; rotated points
	s3d_t_points	dd ?			; 2d points

	s3d_faces	dd ?			; faces
	s3d_depths	dd ?			; faces' depths
	s3d_order	dd ?			; order table

	s3d_point_nrm	dd ?			; point normals
	s3d_r_point_nrm	dd ?			; rotated point normals

	s3d_tex_coords	dd ?			; texture coordinates

	s3d_bmap	dd ?			; bumpmap / texture
	s3d_emap	dd ?			; environment map
ends

face struc
	face_v1		dw ?
	face_v2		dw ?
	face_v3		dw ?
ends
