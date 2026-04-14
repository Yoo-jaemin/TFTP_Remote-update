#define __GDI_C__
    #include "gdi.h"
#undef  __GDI_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "lcd.h"
#include "vimage.h"
/* End Include */

/* Start Define */
#define POLY_X(Z)							((int32_t)((Points + (Z))->X))
#define POLY_Y(Z)							((int32_t)((Points + (Z))->Y))

#define ABS(X)								((X) > 0 ? (X) : -(X))
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void DrawChar( uint16_t Xpos, uint16_t Ypos, const uint8_t *c );
static void SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
static void GDI_Color_Init( void );
/* End Function */

/* Start Variable */
static LCD_DriveType	*lcd_drv = &LcdDrive;
static bool				IsBeforeDot = false;
/* End Variable */

/**
  * @brief  Initializes the LCD.
  * @retval LCD state
  */

bool GDI_Init( void )
{ 
	bool	err = false;
	
	GDI_Color_Init();
		
	DrawProp.BackColor = RGB.BLACK;
	DrawProp.TextColor = RGB.WHITE;
	DrawProp.pFont     = &Font11x16;
		
	err = lcd_drv->Init();
	
	if( err == false ){
		vDrawImg.Intro( 0, 0 );
	}
	
	return err;
}

/**
  * @brief  Gets the LCD X size. 
  * @retval Used LCD X size
  */
uint32_t GDI_GetXSize(void)
{
  return(lcd_drv->GetLcdPixelWidth());
}

/**
  * @brief  Gets the LCD Y size.  
  * @retval Used LCD Y size
  */
uint32_t GDI_GetYSize(void)
{
  return(lcd_drv->GetLcdPixelHeight());
}

/**
  * @brief  Gets the LCD text color. 
  * @retval Used text color.
  */
uint16_t GDI_GetTextColor(void)
{
  return DrawProp.TextColor;
}

/**
  * @brief  Gets the LCD background color.
  * @retval Used background color
  */
uint16_t GDI_GetBackColor(void)
{
  return DrawProp.BackColor;
}

/**
  * @brief  Sets the LCD text color.
  * @param  Color: Text color code RGB(5-6-5)
  */
void GDI_SetTextColor(uint16_t Color)
{
  DrawProp.TextColor = Color;
}

/**
  * @brief  Sets the LCD background color.
  * @param  Color: Background color code RGB(5-6-5)
  */
void GDI_SetBackColor(uint16_t Color)
{
  DrawProp.BackColor = Color;
}

/**
  * @brief  Sets the LCD text font.
  * @param  fonts: Font to be used
  */
void GDI_SetFont(sFONT *fonts)
{
  DrawProp.pFont = fonts;
}

/**
  * @brief  Gets the LCD text font.
  * @retval Used font
  */
sFONT *GDI_GetFont(void)
{
  return DrawProp.pFont;
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: Color of the background
  */
void GDI_Clear(uint16_t Color)
{ 
  uint32_t counter = 0;
  uint32_t color_backup = DrawProp.TextColor; 
  DrawProp.TextColor = Color;
  
  for(counter = 0; counter < GDI_GetYSize(); counter++)
  {
    GDI_DrawHLine(0, counter, GDI_GetXSize());
  }
  DrawProp.TextColor = color_backup; 
  GDI_SetTextColor(DrawProp.TextColor);
}

/**
  * @brief  Clears the selected line.
  * @param  Line: Line to be cleared
  *          This parameter can be one of the following values:
  *            @arg  0..9: if the Current fonts is Font11x16x24
  *            @arg  0..19: if the Current fonts is Font07x12x12 or Font08x12
  *            @arg  0..29: if the Current fonts is Font08x8
  */
void GDI_ClearStringLine(uint16_t Line)
{ 
  uint32_t color_backup = DrawProp.TextColor; 
  DrawProp.TextColor = DrawProp.BackColor;
    
  /* Draw a rectangle with background color */
  GDI_FillRect(0, (Line * DrawProp.pFont->Height), GDI_GetXSize(), DrawProp.pFont->Height);
  
  DrawProp.TextColor = color_backup;
  GDI_SetTextColor(DrawProp.TextColor);
}

/**
  * @brief  Displays one character.
  * @param  Xpos: Start column address
  * @param  Ypos: Line where to display the character shape.
  * @param  Ascii: Character ascii code
  *           This parameter must be a number between Min_Data = 0x20 and Max_Data = 0x7E 
  */
void GDI_DisplayChar( uint16_t Xpos, uint16_t Ypos, uint8_t Ascii )
{
	DrawChar(Xpos, Ypos, &DrawProp.pFont->pTable[(Ascii-' ') * DrawProp.pFont->Height * ((DrawProp.pFont->Width + 7) / 8)]);
}

/**
  * @brief  Displays characters on the LCD.
  * @param  Xpos: X position (in pixel)
  * @param  Ypos: Y position (in pixel)   
  * @param  Text: Pointer to string to display on LCD
  * @param  Mode: Display mode
  *          This parameter can be one of the following values:
  *            @arg  GDI_STR_ALIGN_C
  *            @arg  GDI_STR_ALIGN_R
  *            @arg  GDI_STR_ALIGN_L   
  */
void GDI_StringColor( uint16_t Xpos, uint16_t Ypos, GDI_LineModeType Mode, uint16_t color, char *str )
{
	GDI_SetTextColor( color );
	GDI_String( Xpos, Ypos, Mode, str );
}

/**
  * @brief  Displays characters on the LCD.
  * @param  Xpos: X position (in pixel)
  * @param  Ypos: Y position (in pixel)   
  * @param  Text: Pointer to string to display on LCD
  * @param  Mode: Display mode
  *          This parameter can be one of the following values:
  *            @arg  GDI_STR_ALIGN_C
  *            @arg  GDI_STR_ALIGN_R
  *            @arg  GDI_STR_ALIGN_L   
  */
void GDI_String( uint16_t Xpos, uint16_t Ypos, GDI_LineModeType Mode, char *Text )
{
	uint16_t	refcolumn				= 1;
	uint16_t	i						= 0;
	uint32_t	size					= 0;
	char		*ptr					= Text;
	bool		IsMesuringFontDotMode	= false;
	
	/* Get the text size */
	while( *ptr++ ){
		if( (*ptr == '.') && (DrawProp.pFont->pTable == Font29x46.pTable) ){
			IsMesuringFontDotMode = true;
		}
		size++;
	}
	
	switch( Mode ){
		case GDI_STR_ALIGN_C	: refcolumn = Xpos - size * DrawProp.pFont->Width / 2;	break;
		case GDI_STR_ALIGN_L	: refcolumn = Xpos;										break;
		case GDI_STR_ALIGN_R	: refcolumn = Xpos - size * DrawProp.pFont->Width;		break;
		default					: refcolumn = Xpos;										break;
	}
	
	if( DrawProp.pFont->pTable == Font29x46.pTable ){
		if( IsMesuringFontDotMode == true )	{ refcolumn += 26; }
		//else								{ refcolumn -=  3; }
	}
	
	/* Send the string character by character on LCD */
	while( (*Text != 0) & (((GDI_GetXSize() - (i * DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width) )
	{                       
		if ( (IsMesuringFontDotMode == true) && (*(Text+1) == '.') ){
			IsBeforeDot = true;
		}
		else if( (IsMesuringFontDotMode == true) && (*Text == '.') ){
			refcolumn -= 3;
		}
		
		/* Display one character on LCD */
		GDI_DisplayChar( refcolumn, Ypos, *Text );
		/* Decrement the column position by 16 */
		
		if( (IsMesuringFontDotMode == true) && (*Text == '.') )	{ refcolumn += 6;					  }
		else													{ refcolumn += DrawProp.pFont->Width; }
		
		/* Point on the next character */
		Text++;
		i++;
	}
}

/**
  * @brief  Displays a character on the LCD.
  * @param  Line: Line where to display the character shape
  *          This parameter can be one of the following values:
  *            @arg  0..9: if the Current fonts is Font11x16x24  
  *            @arg  0..19: if the Current fonts is Font07x12x12 or Font08x12
  *            @arg  0..29: if the Current fonts is Font08x8
  * @param  ptr: Pointer to string to display on LCD
  */
void GDI_StringAtLine( uint16_t Line, char *ptr )
{
	GDI_String( 0, LINE(Line), GDI_STR_ALIGN_L, ptr );
}

/**
  * @brief  Reads an LCD pixel.
  * @param  Xpos: X position 
  * @param  Ypos: Y position 
  * @retval RGB pixel color
  */
uint16_t GDI_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint16_t ret = 0;
  
  if(lcd_drv->ReadPixel != NULL)
  {
    ret = lcd_drv->ReadPixel(Xpos, Ypos);
  }
    
  return ret;
}

/**
  * @brief  Draws a pixel on LCD.
  * @param  Xpos: X position 
  * @param  Ypos: Y position
  * @param  RGB_Code: Pixel color in RGB mode (5-6-5)  
  */
void GDI_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code)
{
  if(lcd_drv->WritePixel != NULL)
  {
    lcd_drv->WritePixel(Xpos, Ypos, RGB_Code);
  }
}
  
/**
  * @brief  Draws an horizontal line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  */
void GDI_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t index = 0;
  
  if(lcd_drv->DrawHLine != NULL)
  {
    lcd_drv->DrawHLine(DrawProp.TextColor, Xpos, Ypos, Length);
  }
  else
  {
    for(index = 0; index < Length; index++)
    {
      GDI_DrawPixel((Xpos + index), Ypos, DrawProp.TextColor);
    }
  }
}

/**
  * @brief  Draws a vertical line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  */
void GDI_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t index = 0;
  
  if(lcd_drv->DrawVLine != NULL)
  {
    lcd_drv->DrawVLine(DrawProp.TextColor, Xpos, Ypos, Length);
  }
  else
  {
    for(index = 0; index < Length; index++)
    {
      GDI_DrawPixel(Xpos, Ypos + index, DrawProp.TextColor);
    }
  }
}
  
/**
  * @brief  Draws an horizontal line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  * @param  Color: Line color
  */
void GDI_DrawHLineColor( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color )
{
	uint32_t	index = 0;
	
	GDI_SetTextColor( Color );
	
	if( lcd_drv->DrawHLine != NULL ){
		lcd_drv->DrawHLine( DrawProp.TextColor, Xpos, Ypos, Length );
	}
	else {
		for( index=0; index<Length; index++ ){
		  GDI_DrawPixel( (Xpos + index), Ypos, DrawProp.TextColor );
		}
	}
}
  
/**
  * @brief  Draws an horizontal line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  * @param  Color: Line color
  */
void GDI_DrawVLineColor( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color )
{
	uint32_t	index = 0;
	
	GDI_SetTextColor( Color );
	
	if( lcd_drv->DrawVLine != NULL ){
		lcd_drv->DrawVLine( DrawProp.TextColor, Xpos, Ypos, Length );
	}
	else {
		for( index=0; index<Length; index++ ){
			GDI_DrawPixel( Xpos, Ypos + index, DrawProp.TextColor );
		}
	}
}

/**
  * @brief  Draws an uni-line (between two points).
  * @param  x1: Point 1 X position
  * @param  y1: Point 1 Y position
  * @param  x2: Point 2 X position
  * @param  y2: Point 2 Y position
  */
void GDI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    GDI_DrawPixel(x, y, DrawProp.TextColor);  /* Draw the current pixel */
    num += numadd;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

/**
  * @brief  Draws a rectangle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Width: Rectangle width  
  * @param  Height: Rectangle height
  */
void GDI_DrawRectColor( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color )
{
	GDI_SetTextColor( Color );
	
	/* Draw horizontal lines */
	GDI_DrawHLine( Xpos, Ypos, Width );
	GDI_DrawHLine( Xpos, (Ypos + Height - 1), Width );
	
	/* Draw vertical lines */
	GDI_DrawVLine( Xpos, Ypos, Height );
	GDI_DrawVLine( (Xpos + Width - 1), Ypos, Height );
}

void GDI_DrawRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height )
{
	/* Draw horizontal lines */
	GDI_DrawHLine( Xpos, Ypos, Width );
	GDI_DrawHLine( Xpos, (Ypos + Height - 1), Width );
	
	/* Draw vertical lines */
	GDI_DrawVLine( Xpos, Ypos, Height );
	GDI_DrawVLine( (Xpos + Width - 1), Ypos, Height );
}
                            
/**
  * @brief  Draws a circle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Radius: Circle radius
  */
void GDI_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;       /* Decision Variable */ 
  uint32_t  CurX;   /* Current X Value */
  uint32_t  CurY;   /* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    GDI_DrawPixel((Xpos + CurX), (Ypos - CurY), DrawProp.TextColor);

    GDI_DrawPixel((Xpos - CurX), (Ypos - CurY), DrawProp.TextColor);

    GDI_DrawPixel((Xpos + CurY), (Ypos - CurX), DrawProp.TextColor);

    GDI_DrawPixel((Xpos - CurY), (Ypos - CurX), DrawProp.TextColor);

    GDI_DrawPixel((Xpos + CurX), (Ypos + CurY), DrawProp.TextColor);

    GDI_DrawPixel((Xpos - CurX), (Ypos + CurY), DrawProp.TextColor);

    GDI_DrawPixel((Xpos + CurY), (Ypos + CurX), DrawProp.TextColor);

    GDI_DrawPixel((Xpos - CurY), (Ypos + CurX), DrawProp.TextColor);   

    /* Initialize the font */
    GDI_SetFont(&LCD_DEFAULT_FONT);

    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  } 
}

/**
  * @brief  Draws an poly-line (between many points).
  * @param  Points: Pointer to the points array
  * @param  PointCount: Number of points
  */
void GDI_DrawPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  GDI_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    GDI_DrawLine(X, Y, Points->X, Points->Y);
  }
  
}

/**
  * @brief  Draws an ellipse on LCD.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  XRadius: Ellipse X radius
  * @param  YRadius: Ellipse Y radius
  */
void GDI_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float K = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  K = (float)(rad2/rad1);
  
  do {      
    GDI_DrawPixel((Xpos-(uint16_t)(x/K)), (Ypos+y), DrawProp.TextColor);
    GDI_DrawPixel((Xpos+(uint16_t)(x/K)), (Ypos+y), DrawProp.TextColor);
    GDI_DrawPixel((Xpos+(uint16_t)(x/K)), (Ypos-y), DrawProp.TextColor);
    GDI_DrawPixel((Xpos-(uint16_t)(x/K)), (Ypos-y), DrawProp.TextColor);      
    
    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;     
  }
  while (y <= 0);
}

/**
  * @brief  Draws a bitmap picture (16 bpp).
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD
  * @param  pbmp: Pointer to Bmp picture address.
  */

void GDI_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t height = 0;
  uint32_t width  = 0;
  
  /* Read bitmap width */
  width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);

  /* Read bitmap height */
  height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);
  
  SetDisplayWindow(Xpos, Ypos, width, height);
  
  if(lcd_drv->DrawBitmap != NULL)
  {
    lcd_drv->DrawBitmap(Xpos, Ypos, pbmp);
  } 
  SetDisplayWindow(0, 0, GDI_GetXSize(), GDI_GetYSize());
}

/**
  * @brief  Draws RGB Image (16 bpp).
  * @param  Xpos:  X position in the LCD
  * @param  Ypos:  Y position in the LCD
  * @param  Xsize: X size in the LCD
  * @param  Ysize: Y size in the LCD
  * @param  pdata: Pointer to the RGB Image address.
  */

void GDI_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata)
{
	SetDisplayWindow( Xpos, Ypos, Xsize, Ysize );
	
	if( lcd_drv->DrawRGBImage != NULL )
	{
		lcd_drv->DrawRGBImage( Xpos, Ypos, Xsize, Ysize, pdata );
	}                               
	
	SetDisplayWindow( 0, 0, GDI_GetXSize(), GDI_GetYSize() );	
}

/**
  * @brief  Draws a full rectangle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Width: Rectangle width  
  * @param  Height: Rectangle height
  */
void GDI_FillRectColor( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color )
{
	GDI_SetTextColor( Color );
	while( Height-- ){ GDI_DrawHLine( Xpos, Ypos++, Width ); }
}

void GDI_FillRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height )
{
	while( Height-- ){ GDI_DrawHLine( Xpos, Ypos++, Width ); }
}

/**
  * @brief  Draws a full circle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Radius: Circle radius
  */
void GDI_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;        /* Decision Variable */ 
  uint32_t  CurX;    /* Current X Value */
  uint32_t  CurY;    /* Current Y Value */ 
  
  D = 3 - (Radius << 1);

  CurX = 0;
  CurY = Radius;
  
  GDI_SetTextColor(DrawProp.TextColor);

  while (CurX <= CurY)
  {
    if(CurY > 0) 
    {
      GDI_DrawHLine(Xpos - CurY, Ypos + CurX, 2*CurY);
      GDI_DrawHLine(Xpos - CurY, Ypos - CurX, 2*CurY);
    }

    if(CurX > 0) 
    {
      GDI_DrawHLine(Xpos - CurX, Ypos - CurY, 2*CurX);
      GDI_DrawHLine(Xpos - CurX, Ypos + CurY, 2*CurX);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }

  GDI_SetTextColor(DrawProp.TextColor);
  GDI_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  Draws a full poly-line (between many points).
  * @param  Points: Pointer to the points array
  * @param  PointCount: Number of points
  */
void GDI_FillPolygon(pPoint Points, uint16_t PointCount)
{
  
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;  
  
  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;
  
  for(counter = 1; counter < PointCount; counter++)
  {
    pixelX = POLY_X(counter);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }
    
    pixelY = POLY_Y(counter);
    if(pixelY < IMAGE_TOP)
    { 
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }  
  
  if(PointCount < 2)
  {
    return;
  }
  
  X_center = (IMAGE_LEFT + IMAGE_RIGHT)/2;
  Y_center = (IMAGE_BOTTOM + IMAGE_TOP)/2;
  
  X_first = Points->X;
  Y_first = Points->Y;
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;    
    
    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    FillTriangle(X_center, X2, X, Y_center, Y2, Y);   
  }
  
  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);   
}

/**
  * @brief  Draws a full ellipse.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  XRadius: Ellipse X radius
  * @param  YRadius: Ellipse Y radius  
  */
void GDI_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float K = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  K = (float)(rad2/rad1);    
  
  do 
  { 
    GDI_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1));
    GDI_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1));
    
    e2 = err;
    if (e2 <= x) 
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

/**
  * @brief  Enables the display.
  */
void GDI_DisplayOn(void)
{
  lcd_drv->DisplayOn();
}

/**
  * @brief  Disables the display.
  */
void GDI_DisplayOff(void)
{
  lcd_drv->DisplayOff();
}

/******************************************************************************
                            Static Function
*******************************************************************************/

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: Line where to display the character shape
  * @param  Ypos: Start column address
  * @param  c: Pointer to the character data
  */
static void DrawChar( uint16_t Xpos, uint16_t Ypos, const uint8_t *c )
{
	uint32_t	i = 0, j = 0, s = 0;
	uint16_t	height, width;
	uint8_t		offset;
	uint8_t		*pchar;
	uint32_t	line[2];
	uint32_t	pixel;
	uint32_t	color;
	uint32_t	widthbyte;
	bool		IsMesuringFontDotMode = false;
	 
	if( c == (Font29x46.pTable + 2576) ){
		IsMesuringFontDotMode = true;
		width	= 6;
	}
	else if( IsBeforeDot == true ){
		IsBeforeDot = false;
		width	= DrawProp.pFont->Width - 3;
	}
	else {                               
		width	= DrawProp.pFont->Width;
	}
	
	height		= DrawProp.pFont->Height;
	widthbyte	= ( width + 7 ) / 8;
	offset		= 8 * widthbyte - width;
	
	for( i=0; i<height; i++ ){
		
		if( IsMesuringFontDotMode == true )	{ pchar = ( (uint8_t *)c + (widthbyte + 3) * i ); }
		else								{ pchar = ( (uint8_t *)c + (widthbyte    ) * i ); }
		
		switch( widthbyte ){
			case  1:
				line[0] = (pchar[0]);
				break;
				
			case  2:
				line[0] = (pchar[0] <<  8) | (pchar[1]);
				break;
				
			case  3:
				line[0] = (pchar[0] << 16) | (pchar[1] <<  8) | (pchar[2]);
				break;
				
			case  4:
				line[0] = (pchar[0] << 24) | (pchar[1] << 16) | (pchar[2] << 8) | (pchar[3]);
				break;
				
			case  5:
				line[0] = (pchar[0] << 24) | (pchar[1] << 16) | (pchar[2] << 8) | (pchar[3]);
				line[1] = (pchar[4]);
				break;
				
			case  6:
				line[0] = (pchar[0] << 24) | (pchar[1] << 16) | (pchar[2] << 8) | (pchar[3]);
				line[1] = (pchar[4] <<  8) | (pchar[5]);
				break;
				
			case  7:
				line[0] = (pchar[0] << 24) | (pchar[1] << 16) | (pchar[2] << 8) | (pchar[3]);
				line[1] = (pchar[4] << 16) | (pchar[5] <<  8) | (pchar[6] << 8);
				break;
				
			//case  8:
			default:
				line[0] = (pchar[0] << 24) | (pchar[1] << 16) | (pchar[2] << 8) | (pchar[3]);
				line[1] = (pchar[4] << 24) | (pchar[5] << 16) | (pchar[6] << 8) | (pchar[7]);
				break;
		}
		
		for( j=0; j<width; j++ ){
			
			s = ( width - j + offset - 1 );
			
			if( j < 32 ){ pixel = line[0]; if( widthbyte > 4 ){ s -= (8 * (widthbyte - 4)); } }
			else		{ pixel = line[1]; }
			
			if( pixel & (1 << s) )	{ color = DrawProp.TextColor; }
			else					{ color = DrawProp.BackColor; }
			
			GDI_DrawPixel( (Xpos + j), Ypos, color );
		}
		Ypos++;
	}
}

/**
  * @brief  Sets display window.
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height  
  */
static void SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  if(lcd_drv->SetDisplayWindow != NULL)
  {
    lcd_drv->SetDisplayWindow(Xpos, Ypos, Width, Height);
  }  
}

/**
  * @brief  Fills a triangle (between 3 points).
  * @param  x1: Point 1 X position
  * @param  y1: Point 1 Y position
  * @param  x2: Point 2 X position
  * @param  y2: Point 2 Y position
  * @param  x3: Point 3 X position
  * @param  y3: Point 3 Y position
  */
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    GDI_DrawLine(x, y, x3, y3);
    
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  } 
}

static void GDI_Color_Init( void )
{
	RGB.RED		        		= ( 0xF800 );
	RGB.CRIMSON		    		= ( 0xD8A7 );
	RGB.FIREBRICK	    		= ( 0xB104 );
	RGB.MAROON		    		= ( 0x8000 );
	RGB.DARKRED		    		= ( 0x8800 );
	RGB.BROWN		    		= ( 0xA145 );
	RGB.SIENNA		    		= ( 0xA285 );
	RGB.SADDLEBROWN				= ( 0x8A22 );
	RGB.INDIANRED				= ( 0xCAEB );
	RGB.ROSYBROWN				= ( 0xBC71 );
	RGB.LIGHTCORAL				= ( 0xF410 );
	RGB.SALMON		    		= ( 0xFC0E );
	RGB.DARKSALMON				= ( 0xECAF );
	RGB.CORAL		    		= ( 0xFBEA );
	RGB.TOMATO		    		= ( 0xFB08 );
	RGB.SANDYBROWN				= ( 0xF52C );
	RGB.LIGHTSALMON				= ( 0xFD0F );
	RGB.PERU		    		= ( 0xCC27 );
	RGB.CHOCOLATE				= ( 0xD343 );
	RGB.ORANGERED				= ( 0xFA20 );
	RGB.ORANGE		    		= ( 0xFD20 );
	RGB.DARKORANGE				= ( 0xFC60 );
	RGB.TAN		        		= ( 0xD5B1 );
	RGB.PEACHPUFF				= ( 0xFED7 );
	RGB.BISQUE		    		= ( 0xFF38 );
	RGB.MOCCASIN				= ( 0xFF36 );
	RGB.NAVAJOWHITE				= ( 0xFEF5 );
	RGB.WHEAT		    		= ( 0xF6F6 );
	RGB.BURLYWOOD				= ( 0xDDD0 );
	RGB.DARKGOLDENROD			= ( 0xBC21 );
	RGB.GOLDENROD				= ( 0xDD24 );
	RGB.GOLD		    		= ( 0xFEA0 );
	RGB.YELLOW		    		= ( 0xFFE0 );
	RGB.LIGHTGOLDENRODYELLOW	= ( 0xFFDA );
	RGB.PALEGOLDENROD			= ( 0xEF55 );
	RGB.KHAKI		    		= ( 0xF731 );
	RGB.DARKKHAKI				= ( 0xBDAD );
	RGB.LAWNGREEN				= ( 0x7FE0 );
	RGB.GREENYELLOW				= ( 0xAFE5 );
	RGB.CHARTREUSE				= ( 0x7FE0 );
	RGB.LIME		    		= ( 0x07E0 );
	RGB.LIMEGREEN				= ( 0x3666 );
	RGB.YELLOWGREEN				= ( 0x9E66 );
	RGB.OLIVE		    		= ( 0x8400 );
	RGB.OLIVEDRAB				= ( 0x6C64 );
	RGB.DARKOLIVEGREEN			= ( 0x5345 );
	RGB.FORESTGREEN				= ( 0x2444 );
	RGB.DARKGREEN				= ( 0x0320 );
	RGB.GREEN		    		= ( 0x07E0 );
	RGB.SEAGREEN				= ( 0x2C4A );
	RGB.MEDIUMSEAGREEN			= ( 0x3D8E );
	RGB.DARKSEAGREEN			= ( 0x8DF1 );
	RGB.LIGHTGREEN				= ( 0x9772 );
	RGB.PALEGREEN				= ( 0x9FD3 );
	RGB.SPRINGGREEN				= ( 0x07EF );
	RGB.MEDIUMSPRINGGREEN		= ( 0x07D3 );
	RGB.TEAL		    		= ( 0x0410 );
	RGB.DARKCYAN				= ( 0x0451 );
	RGB.LIGHTSEAGREEN			= ( 0x2595 );
	RGB.MEDIUMAQUAMARINE		= ( 0x6675 );
	RGB.CADETBLUE				= ( 0x5CF4 );
	RGB.STEELBLUE				= ( 0x4416 );
	RGB.AQUAMARINE				= ( 0x7FFA );
	RGB.POWDERBLUE				= ( 0xB71C );
	RGB.PALETURQUOISE			= ( 0xAF7D );
	RGB.LIGHTBLUE				= ( 0xAEDC );
	RGB.LIGHTSTEELBLUE			= ( 0xB63B );
	RGB.SKYBLUE		    		= ( 0x867D );
	RGB.LIGHTSKYBLUE			= ( 0x867F );
	RGB.MEDIUMTURQUOISE			= ( 0x4E99 );
	RGB.TURQUOISE				= ( 0x471A );
	RGB.DARKTURQUOISE			= ( 0x067A );
	RGB.AQUA		    		= ( 0x07FF );
	RGB.CYAN		    		= ( 0x07FF );
	RGB.DEEPSKYBLUE				= ( 0x05FF );
	RGB.DODGERBLUE				= ( 0x1C9F );
	RGB.CORNFLOWERBLUE			= ( 0x64BD );
	RGB.ROYALBLUE				= ( 0x435C );
	RGB.BLUE		    		= ( 0x001F );
	RGB.MEDIUMBLUE				= ( 0x0019 );
	RGB.NAVY		    		= ( 0x0010 );
	RGB.DARKBLUE				= ( 0x0011 );
	RGB.MIDNIGHTBLUE			= ( 0x18CE );
	RGB.DARKSLATEBLUE			= ( 0x49F1 );
	RGB.SLATEBLUE				= ( 0x6AD9 );
	RGB.MEDIUMSLATEBLUE			= ( 0x7B5D );
	RGB.MEDIUMPURPLE			= ( 0x939B );
	RGB.DARKORCHID				= ( 0x9999 );
	RGB.DARKVIOLET				= ( 0x901A );
	RGB.BLUEVIOLET				= ( 0x895C );
	RGB.MEDIUMORCHID			= ( 0xBABA );
	RGB.PLUM		    		= ( 0xDD1B );
	RGB.LAVENDER				= ( 0xE73F );
	RGB.THISTLE		    		= ( 0xDDFB );
	RGB.ORCHID		    		= ( 0xDB9A );
	RGB.VIOLET		    		= ( 0xEC1D );
	RGB.INDIGO		    		= ( 0x4810 );
	RGB.DARKMAGENTA				= ( 0x8811 );
	RGB.PURPLE		    		= ( 0x8010 );
	RGB.MEDIUMVIOLETRED			= ( 0xC0B0 );
	RGB.DEEPPINK				= ( 0xF8B2 );
	RGB.FUCHSIA		    		= ( 0xF81F );
	RGB.MAGENTA		    		= ( 0xF81F );
	RGB.HOTPINK		    		= ( 0xFB56 );
	RGB.PALEVIOLETRED			= ( 0xDB92 );
	RGB.LIGHTPINK				= ( 0xFDB8 );
	RGB.PINK		    		= ( 0xFE19 );
	RGB.MISTYROSE				= ( 0xFF3C );
	RGB.BLANCHEDALMOND			= ( 0xFF59 );
	RGB.LIGHTYELLOW				= ( 0xFFFC );
	RGB.CORNSILK				= ( 0xFFDB );
	RGB.ANTIQUEWHITE			= ( 0xFF5A );
	RGB.PAPAYAWHIP				= ( 0xFF7A );
	RGB.LEMONCHIFFON			= ( 0xFFD9 );
	RGB.BEIGE		    		= ( 0xF7BB );
	RGB.LINEN		    		= ( 0xFF9C );
	RGB.OLDLACE		    		= ( 0xFFBC );
	RGB.LIGHTCYAN				= ( 0xE7FF );
	RGB.ALICEBLUE				= ( 0xF7DF );
	RGB.WHITESMOKE				= ( 0xF7BE );
	RGB.LAVENDERBLUSH			= ( 0xFF9E );
	RGB.FLORALWHITE				= ( 0xFFDE );
	RGB.MINTCREAM				= ( 0xF7FF );
	RGB.GHOSTWHITE				= ( 0xFFDF );
	RGB.HONEYDEW				= ( 0xF7FE );
	RGB.SEASHELL				= ( 0xFFBD );
	RGB.IVORY		    		= ( 0xFFFE );
	RGB.AZURE		    		= ( 0xF7FF );
	RGB.SNOW		    		= ( 0xFFDF );
	RGB.WHITE		    		= ( 0xFFFF );
	RGB.GAINSBORO				= ( 0xDEFB );
	RGB.LIGHTGRAY				= ( 0xD69A );
	RGB.SILVER		    		= ( 0xC618 );
	RGB.DARKGRAY				= ( 0xAD55 );
	RGB.LIGHTSLATEGRAY			= ( 0x7453 );
	RGB.SLATEGRAY				= ( 0x7412 );
	RGB.GRAY		    		= ( 0x8410 );
	RGB.DIMGRAY		    		= ( 0x6B4D );
	RGB.DARKSLATEGRAY			= ( 0x2A69 );
	RGB.SELECTED				= ( 0x00E8 );
	RGB.BLACK		    		= ( 0x0000 );
}

/**
  * @}
  */  
  
/**
  * @}
  */ 
  
/**
  * @}
  */     

/**
  * @}
  */  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

