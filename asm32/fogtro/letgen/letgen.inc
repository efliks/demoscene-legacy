
; used by letters generator
tiny_struct3d struc
	ts3d_n_points	dd ?
	ts3d_n_faces	dd ?
			dd ?
	ts3d_points	dd ?
			dd ?
			dd ?
	ts3d_faces	dd ?
ends

polygon struc
	poly_v1		dw ?
	poly_v2		dw ?
	poly_v3		dw ?
	poly_v4		dw ?
ends

;------------------------------------------------------------
