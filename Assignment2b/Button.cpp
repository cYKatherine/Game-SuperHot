/*	FIT2096 - Example Code
*	Button.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of Button.h
*/

#include "Button.h"

Button::Button(int width, int height, Texture* texture, LPCWSTR text, Vector2 position, 
	SpriteBatch* spriteBatch, SpriteFont* font, InputController* input, std::function<void()> onClickFunction)
{
	//Setting basic values
	m_width = width;
	m_height = height;
	m_buttonTexture = texture;
	m_text = text;
	m_position = position;
	m_textColour = Color(0.2f, 0.2f, 0.2f, 1.0f);
	m_isEnabled = true;

	//Setting the objects we'll be using to do stuff
	m_spriteBatch = spriteBatch;
	m_font = font;
	m_input = input;

	//Here we are calculating some new values
	m_origin = Vector2(m_width / 2.0f, m_height / 2.0f);	//The first value to calculate is the origin, this defines what part of the texture will be placed at m_position
															//Here we are saying that we want the centre of the texture to be placed at m_position and rest should be placed
															//around that point
	m_textureWindow.top = 0;								//The next value to calculate is the RECT that will define what part of the texture is displayed
	m_textureWindow.left = 0;								//The important values are the top and bottom, which we set to 0 and to m_height
	m_textureWindow.right = m_width;						//Remember: m_height is only half of the height of the actual texture, this means the we will only be
	m_textureWindow.bottom = m_height;						//displaying the top half of the texture

	Vector2 stringSize = m_font->MeasureString(m_text);						//The final value to calculate is the origin of the text. To do this we need to know how big
	m_textOrigin = Vector2((int)stringSize.x / 2, (int)stringSize.y / 2);	//the string will be when it's rendered with our font. We then divide this by 2 to get an
																			//origin point that is in the middle of the string.
	
	//We also need to make sure we remember our function object
	m_onClickFunction = onClickFunction;
}

Button::~Button()
{

}

void Button::Update()
{
	//The following if statements use m_width / 2 and m_height / 2 a lot so we calculate them once and save the values
	int halfWidth = m_width / 2;
	int halfHeight = m_height / 2;

	//Now the origin point is in the middle of the texture so when we look at m_position we're talking about the position of the middle of the button
	//So this means that we want to know if the mouse is on the button then the X position of the mouse needs to be between:
	//m_position.x - halfWidth and m_position.x + halfWidth
	//and the Y position of the mouse needs to be between:
	//m_position.y - halfHeight and m_position.y + halfHeight

	if (m_input->GetMouseX() > (m_position.x - halfWidth) && m_input->GetMouseX() < (m_position.x + halfWidth) &&
		m_input->GetMouseY() > (m_position.y - halfHeight) && m_input->GetMouseY() < (m_position.y + halfHeight))
	{
		
		//If our mouse if over the button then we change the section of the texture so that we see the bottom section
		m_textureWindow.top = m_height;
		m_textureWindow.bottom = m_height * 2;

		if (m_isEnabled)
		{
			//Now we check to see if the Left Mouse button has been clicked
			if (m_input->GetMouseUp(LEFT_MOUSE))
			{
				//If it has, run our function object!
				m_onClickFunction();
			}
		}
			
	}
	else
	{
		//If our mouse isn't over the button then set it so we can see the top section of the texture
		m_textureWindow.top = 0;
		m_textureWindow.bottom = m_height;
	}
}

void Button::Render()
{
	float alpha = 1.0f;

	if (!m_isEnabled)
		alpha = 0.3f;

	//We now render the texture at our position, using the texture window and origin that we defined and a White colour
	m_spriteBatch->Draw(m_buttonTexture->GetShaderResourceView(), m_position, &m_textureWindow, Color(1.0f, 1.0f, 1.0f, alpha), 0, m_origin);

	//Then we draw the text at our position, using the origin we calculated and the colour that was set
	m_font->DrawString(m_spriteBatch, m_text, m_position, m_textColour, 0, m_textOrigin);
}