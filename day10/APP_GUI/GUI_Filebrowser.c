#include "GUI_Filebrowser.h"
#include "MV_Support/MV_BMP.h"
#include "MV_Support/MV_PPM.h"
#include "MV_Support/MV_TXT.h"
#include "MV_Core/MV_Haar.h"
GUI_RootObj_t FilebrowserRoot = {0};
static GUI_SubObj_t *vfilelist;
static GUI_SubObj_t *vsdbar;
static GUI_SubObj_t *vwinpic;
static GUI_SubObj_t *vimgpic;
static GUI_SubObj_t *vwintxt;
static image_t imgpic = {0};
static lv_img_dsc_t lvimgpic = {
  .header.always_zero = 0,
  .header.w = 0,
  .header.h = 0,
  .data_size = 0,
  .header.cf = LV_IMG_CF_TRUE_COLOR,
  .data = NULL,
};
static void file_close_action_animation_cb(lv_obj_t * win)
{
    if(win == FilebrowserRoot.rootobj)
    {
        GUI_Manage_DelRoot(&FilebrowserRoot);
    }
    else if(vwinpic)
    {
        if(win == vwinpic->body)
        {
            GUI_Manage_DelSub(&FilebrowserRoot,vimgpic->id);
            vimgpic = NULL;
            RTE_MEM_Free(MEM_AXIM,lvimgpic.data);
            lvimgpic.data = NULL;
            RTE_MEM_Free(MEM_AXIM,imgpic.pixels);
            imgpic.pixels = NULL;
            GUI_Manage_DelSub(&FilebrowserRoot,vwinpic->id);
            vwinpic = NULL;
        }
    }
    else if(vwintxt)
    {
        if(win == vwintxt->body)
        {
            GUI_Manage_DelSub(&FilebrowserRoot,vwintxt->id);
            vwintxt = NULL;
        }
    }
}
static lv_res_t file_close_action_animation(lv_obj_t * btn)
{
	lv_obj_t * win = lv_win_get_from_btn(btn);
	/* Float in the button using a built-in function
	* Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(win, LV_ANIM_FLOAT_TOP | LV_ANIM_OUT, 300, 50, file_close_action_animation_cb);
	return LV_RES_OK;
}
static lv_res_t list_btn_action(lv_obj_t * btn);
static void file_refresh_list(char **Directory,uint8_t DirectoryNum)
{
	lv_list_add(vfilelist->body, SYMBOL_HOME, "...", list_btn_action);
	if(DirectoryNum != 0)
	{
		for(uint8_t i=0;i<DirectoryNum;i++)
		{
			if(strstr(Directory[i],"."))
			{
				if(strstr(Directory[i],".txt"))
					lv_list_add(vfilelist->body, SYMBOL_FILE, Directory[i], list_btn_action);
				else if(strstr(Directory[i],".bmp")||strstr(Directory[i],".jpg")||strstr(Directory[i],".ppm")||strstr(Directory[i],".pgm"))
				{
					lv_list_add(vfilelist->body, SYMBOL_IMAGE, Directory[i], list_btn_action);
				}
				else if(strstr(Directory[i],".mp3"))
					lv_list_add(vfilelist->body, SYMBOL_AUDIO, Directory[i], list_btn_action);
				else if(strstr(Directory[i],".jpg"))
					break;
				else
					lv_list_add(vfilelist->body, SYMBOL_FILE, Directory[i], list_btn_action);
			}
			else
			{
				lv_list_add(vfilelist->body, SYMBOL_DIRECTORY, Directory[i], list_btn_action);
			}
		}
	}
	lv_obj_animate(vfilelist->body, LV_ANIM_GROW_H | LV_ANIM_IN, 300, 50, NULL);
}
static lv_res_t list_btn_action(lv_obj_t * btn)
{
	char btnlabel[128] = {0};
	strcpy(btnlabel,lv_label_get_text(lv_list_get_btn_label(btn)));
	if(strstr(btnlabel,".") == 0)
	{
	    memset(FileModuleHandle.FatherDir,0,strlen(FileModuleHandle.FatherDir));
        strcat(FileModuleHandle.FatherDir,FileModuleHandle.CurrentDir);
		if(FileModuleHandle.CurrentDir[strlen(FileModuleHandle.CurrentDir)-1]!='/')
			strcat(FileModuleHandle.CurrentDir,"/");
		strcat(FileModuleHandle.CurrentDir,lv_label_get_text(lv_list_get_btn_label(btn)));
		lv_list_clean(vfilelist->body);
        FileModuleHandle.CurrentDirContNum = File_Module_ListDirectory(FileModuleHandle.DirTable,
                                                                   DIR_MAX_NUM,FileModuleHandle.CurrentDir);
        file_refresh_list(FileModuleHandle.DirTable,FileModuleHandle.CurrentDirContNum);
		return LV_RES_INV;
	}
	else if(strstr(btnlabel,"..."))
	{
		lv_list_clean(vfilelist->body);
        FileModuleHandle.CurrentDirContNum = File_Module_ListDirectory(FileModuleHandle.DirTable,
                                                                   DIR_MAX_NUM,FileModuleHandle.FatherDir);
        file_refresh_list(FileModuleHandle.DirTable,FileModuleHandle.CurrentDirContNum);
	    memset(FileModuleHandle.CurrentDir,0,strlen(FileModuleHandle.CurrentDir));
        strcat(FileModuleHandle.CurrentDir,FileModuleHandle.FatherDir);
		if(FileModuleHandle.FatherDir[strlen(FileModuleHandle.FatherDir)-1] == ':')
		{

		}
		else
		{
			for(uint8_t i=strlen(FileModuleHandle.FatherDir)-1;i>0;i--)
			{
				if(FileModuleHandle.FatherDir[i]=='/')
				{
					FileModuleHandle.FatherDir[i] = 0;
					memset(&FileModuleHandle.FatherDir[i],0,DIR_MAX_LEN-strlen(FileModuleHandle.FatherDir));
					if(FileModuleHandle.FatherDir[i-1]==':')
					{
						FileModuleHandle.FatherDir[i] = '/';
					}
					break;
				}
			}
		}
		return LV_RES_INV;
	}
	else if(strstr(btnlabel,".bmp")||strstr(btnlabel,".ppm")||strstr(btnlabel,".pgm"))
	{
		//文件浏览器窗口
		vwinpic = GUI_Manage_NewSub(GUI_WIN,&FilebrowserRoot,lv_scr_act(),NULL);
		lv_obj_set_size(vwinpic->body,LV_HOR_RES-10,LV_VER_RES-10);
		lv_win_set_layout(vwinpic->body, LV_LAYOUT_OFF);
		lv_win_set_title(vwinpic->body,"PIC Viewer");
		lv_win_set_btn_size(vwinpic->body,40);
		lv_win_set_sb_mode(vwinpic->body, LV_SB_MODE_DRAG);
		lv_win_add_btn(vwinpic->body,SYMBOL_CLOSE,file_close_action_animation);
		lv_obj_align(vwinpic->body,lv_scr_act(),LV_ALIGN_CENTER,0,5);
		vimgpic = GUI_Manage_NewSub(GUI_IMG,&FilebrowserRoot,vwinpic->body,NULL);
		if(FileModuleHandle.CurrentDir[strlen(FileModuleHandle.CurrentDir)-1]!='/')
			strcat(FileModuleHandle.CurrentDir,"/");
		strcat(FileModuleHandle.CurrentDir,lv_label_get_text(lv_list_get_btn_label(btn)));
		if(strstr(btnlabel,".bmp"))
            bmp_read(&imgpic,FileModuleHandle.CurrentDir+3);
        else
            ppm_read(&imgpic,FileModuleHandle.CurrentDir+3);

        rectangle_t roi;
        roi.x = 0;
        roi.y = 0;
        roi.w = imgpic.w;
        roi.h = imgpic.h;
        // Detect objects
        //  # Note: Lower scale factor scales-down the image more and detects smaller objects.
        //  # Higher threshold results in a higher detection rate, with more false positives.
        //加载人脸数据
        cascade_t face_cascade = {0};
        imlib_load_cascade(&face_cascade, "frontalface");
        face_cascade.n_stages = 25;
        face_cascade.threshold = 0.75f;
        face_cascade.scale_factor = 1.25f;
        array_t *objects_array = imlib_detect_objects(&imgpic, &face_cascade, &roi);
        RTE_Printf("%d\r\n",array_length(objects_array));
        // Add detected objects to a new Python list...
        for (int i=0; i<array_length(objects_array); i++) {
                rectangle_t *r = array_at(objects_array, i);
                imlib_draw_rectangle(&imgpic, r->x, r->y, r->w,
                r->h, 0xFFFF, 2, false);
        }
        array_free(objects_array);
        lvimgpic.header.w = imgpic.w;
        lvimgpic.header.h = imgpic.h;
        lvimgpic.data_size = imgpic.w*imgpic.h*sizeof(lv_color_int_t);
        lvimgpic.data = RTE_MEM_Alloc0(MEM_AXIM,lvimgpic.data_size);
        if(imgpic.bpp == 2)
        {
            uint16_t *pSrcLine = (uint16_t *)imgpic.pixels;
            lv_color_int_t *pDstLine = (lv_color_int_t *)lvimgpic.data;
            for(uint16_t y=0;y<lvimgpic.header.h;y++)
            {
                for(uint16_t x=0;x<lvimgpic.header.w;x++)
                {
                    uint16_t pixel = (pSrcLine)[x];
        #if LV_COLOR_DEPTH == 32
                    pDstLine[x] = LV_COLOR_MAKE(IM_R565(pixel)<<3,IM_G565(pixel)<<2,IM_B565(pixel)<<3).full;
        #elif LV_COLOR_DEPTH == 16
                    pDstLine[x] = pixel;
        #endif // LV_COLOR_DEPTH
                }
                pSrcLine += imgpic.w;
                pDstLine += lvimgpic.header.w;
            }
        }
        else
        {
            uint8_t *pSrcLine = imgpic.pixels;
            lv_color_int_t *pDstLine = (lv_color_int_t *)lvimgpic.data;
            for(uint16_t y=0;y<lvimgpic.header.h;y++)
            {
                for(uint16_t x=0;x<lvimgpic.header.w;x++)
                {
                    uint8_t pixel = (pSrcLine)[x];
                    pDstLine[x] = IM_RGB565(IM_R825(pixel),IM_G826(pixel),IM_B825(pixel));
                }
                pSrcLine += imgpic.w;
                pDstLine += lvimgpic.header.w;
            }
        }
        lv_img_set_src(vimgpic->body,&lvimgpic);
		for(uint8_t i=strlen(FileModuleHandle.CurrentDir)-1;i>0;i--)
		{
			if(FileModuleHandle.CurrentDir[i]=='/')
			{
				FileModuleHandle.CurrentDir[i] = 0;
				memset(&FileModuleHandle.CurrentDir[i],0,128-strlen(FileModuleHandle.CurrentDir));
				if(FileModuleHandle.CurrentDir[i-1]==':')
				{
					FileModuleHandle.CurrentDir[i] = '/';
				}
				break;
			}
		}
		lv_obj_animate(vwinpic->body, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
		return LV_RES_INV;
	}
	else if(strstr(btnlabel,".txt"))
    {
		//文档浏览器窗口
		vwintxt = GUI_Manage_NewSub(GUI_WIN,&FilebrowserRoot,lv_scr_act(),NULL);
		lv_obj_set_size(vwintxt->body,LV_HOR_RES-10,LV_VER_RES-10);
		lv_win_set_layout(vwintxt->body, LV_LAYOUT_COL_M);
		lv_win_set_title(vwintxt->body,"TXT Viewer");
		lv_win_set_btn_size(vwintxt->body,40);
		lv_win_set_sb_mode(vwintxt->body, LV_SB_MODE_DRAG);
		lv_win_add_btn(vwintxt->body,SYMBOL_CLOSE,file_close_action_animation);
		lv_obj_align(vwintxt->body,lv_scr_act(),LV_ALIGN_CENTER,0,5);
		lv_obj_t * page = lv_page_create(vwintxt->body,NULL);
		lv_obj_set_size(page,lv_obj_get_width(vwintxt->body)-20,lv_obj_get_height(vwintxt->body)-20);
        lv_obj_t * label = lv_label_create(page, NULL);
        lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);             /*Automatically break long lines*/
        lv_obj_set_width(label, lv_page_get_scrl_width(page));          /*Set the width. Lines will break here*/
		if(FileModuleHandle.CurrentDir[strlen(FileModuleHandle.CurrentDir)-1]!='/')
			strcat(FileModuleHandle.CurrentDir,"/");
		strcat(FileModuleHandle.CurrentDir,lv_label_get_text(lv_list_get_btn_label(btn)));
		uint8_t *txtbuf = 0;
        txt_read_all(&txtbuf,FileModuleHandle.CurrentDir+3);
        lv_label_set_text(label,(char *)txtbuf);
        RTE_MEM_Free(MEM_AXIM,txtbuf);
		for(uint8_t i=strlen(FileModuleHandle.CurrentDir)-1;i>0;i--)
		{
			if(FileModuleHandle.CurrentDir[i]=='/')
			{
				FileModuleHandle.CurrentDir[i] = 0;
				memset(&FileModuleHandle.CurrentDir[i],0,128-strlen(FileModuleHandle.CurrentDir));
				if(FileModuleHandle.CurrentDir[i-1]==':')
				{
					FileModuleHandle.CurrentDir[i] = '/';
				}
				break;
			}
		}
		lv_obj_animate(vwintxt->body, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
		return LV_RES_INV;
    }
	return LV_RES_OK;
}
void GUI_Filebrowser_Entrance(void)
{
    //根窗口控件
    GUI_Manage_NewRoot(GUI_WIN,&FilebrowserRoot,lv_scr_act(),NULL,8);
    lv_obj_set_size(FilebrowserRoot.rootobj,LV_HOR_RES,LV_VER_RES);
    lv_win_set_layout(FilebrowserRoot.rootobj, LV_LAYOUT_COL_M); //列布局
    lv_win_set_title(FilebrowserRoot.rootobj,"File Browser");
    lv_win_set_btn_size(FilebrowserRoot.rootobj,40);
    lv_win_set_sb_mode(FilebrowserRoot.rootobj, LV_SB_MODE_DRAG);
    lv_win_add_btn(FilebrowserRoot.rootobj,SYMBOL_CLOSE,file_close_action_animation);
    lv_obj_align(FilebrowserRoot.rootobj,lv_scr_act(),LV_ALIGN_CENTER,0,0);
    //-文件列表
    vfilelist = GUI_Manage_NewSub(GUI_LIST,&FilebrowserRoot,NULL,NULL);
    lv_obj_set_width(vfilelist->body,LV_HOR_RES-40);
    lv_obj_set_height(vfilelist->body, 2 * lv_obj_get_height(FilebrowserRoot.rootobj) / 3);

    //-存储信息
    vsdbar = GUI_Manage_NewSub(GUI_BAR,&FilebrowserRoot,NULL,NULL);
    lv_obj_set_width(vsdbar->body,LV_HOR_RES-40);
    lv_bar_set_value(vsdbar->body, FatFSHandle.Free*100/FatFSHandle.Total);
    //数据更新
	if(Board_SD_IsDetected() == 1)
	{
	    memset(FileModuleHandle.CurrentDir,0,DIR_MAX_LEN);
	    memset(FileModuleHandle.FatherDir,0,DIR_MAX_LEN);
	    strcat(FileModuleHandle.CurrentDir,FatFSHandle.SDPath);
        strcat(FileModuleHandle.FatherDir,FatFSHandle.SDPath);
        FileModuleHandle.CurrentDirContNum = File_Module_ListDirectory(FileModuleHandle.DirTable,
                                                                   DIR_MAX_NUM,FileModuleHandle.CurrentDir);
        file_refresh_list(FileModuleHandle.DirTable,FileModuleHandle.CurrentDirContNum);
	}
	else
		lv_list_add(vfilelist->body, SYMBOL_WARNING, "No SDCard Find!", NULL);

    //风格
    static lv_style_t stylebarbg;
    static lv_style_t stylebarindic;
    lv_style_copy(&stylebarbg, &lv_style_pretty);
    stylebarbg.body.main_color = LV_COLOR_MAKE(119,136,153);
    stylebarbg.body.grad_color = LV_COLOR_MAKE(240,255,255);
    stylebarbg.body.radius = 0;
    stylebarbg.body.border.color = LV_COLOR_WHITE;
    lv_style_copy(&stylebarindic, &lv_style_pretty);
    stylebarindic.body.grad_color =  LV_COLOR_MAKE(127,255,170);
    stylebarindic.body.main_color=  LV_COLOR_MAKE(0,250,154);
    stylebarindic.body.radius = 0;
    stylebarindic.body.shadow.width = 5;
    stylebarindic.body.shadow.color = LV_COLOR_MAKE(255,255,240);
    stylebarindic.body.padding.hor = 1;
    /*Make the indicator a little bit smaller*/
    stylebarindic.body.padding.ver = 1;
    lv_bar_set_style(vsdbar->body, LV_BAR_STYLE_BG, &stylebarbg);
    lv_bar_set_style(vsdbar->body, LV_BAR_STYLE_INDIC, &stylebarindic);
    //动画
    /* Float in the button using a built-in function
     * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(FilebrowserRoot.rootobj, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);


}
