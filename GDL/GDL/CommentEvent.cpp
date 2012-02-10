/** \file
 *  Game Develop
 *  2008-2012 Florian Rival (Florian.Rival@gmail.com)
 */
#if defined(GD_IDE_ONLY)

#include <wx/dcmemory.h>
#include "CommentEvent.h"
#include "GDL/IDE/EventsRenderingHelper.h"
#include "GDL/CommonTools.h"
#include "GDL/tinyxml/tinyxml.h"
#include "GDL/XmlMacros.h"
#include "GDL/IDE/Dialogs/EditComment.h"
#include "GDL/IDE/EventsEditorItemsAreas.h"
#include "GDL/IDE/EventsEditorSelection.h"

void CommentEvent::SaveToXml(TiXmlElement * eventElem) const
{
    TiXmlElement * color;
    color = new TiXmlElement( "Couleur" );
    eventElem->LinkEndChild( color );

    color->SetDoubleAttribute( "r", r );
    color->SetDoubleAttribute( "v", v );
    color->SetDoubleAttribute( "b", b );
    color->SetDoubleAttribute( "textR", textR );
    color->SetDoubleAttribute( "textG", textG );
    color->SetDoubleAttribute( "textB", textB );

    TiXmlElement * com1Elem = new TiXmlElement( "Com1" );
    eventElem->LinkEndChild( com1Elem );
    com1Elem->SetAttribute( "value", com1.c_str() );

    TiXmlElement * com2Elem = new TiXmlElement( "Com2" );
    eventElem->LinkEndChild( com2Elem );
    com2Elem->SetAttribute( "value", com2.c_str() );
}

void CommentEvent::LoadFromXml(const TiXmlElement * eventElem)
{
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "r" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "r", &value ); r = value;}
    else { cout << "Les informations concernant la couleur d'un commentaire manquent."; }
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "v" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "v", &value ); v = value;}
    else { cout <<"Les informations concernant la couleur d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "b" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "b", &value ); b = value;}
    else { cout <<"Les informations concernant la couleur d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Com1" )->Attribute( "value" ) != NULL ) { com1 = eventElem->FirstChildElement( "Com1" )->Attribute( "value" );}
    else { cout <<"Les informations concernant le texte 1 d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Com2" )->Attribute( "value" ) != NULL ) { com2 = eventElem->FirstChildElement( "Com2" )->Attribute( "value" );}
    else { cout <<"Les informations concernant le texte 2 d'un commentaire manquent." ; }

    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "textR" ) != NULL ) { eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "textR", &textR );}
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "textG" ) != NULL ) { eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "textG", &textG );}
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "textB" ) != NULL ) { eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "textB", &textB );}
}

void CommentEvent::EditEvent(wxWindow* parent_, Game & game_, Scene & scene_, MainEditorCommand & mainEditorCommand_)
{
    EditComment dialog(parent_, *this);
    dialog.ShowModal();
}

/**
 * Render the event
 */
void CommentEvent::Render(wxDC & dc, int x, int y, unsigned int width, EventsEditorItemsAreas & areas, EventsEditorSelection & selection)
{
    x += 1; //Small border

    EventsRenderingHelper * renderingHelper = EventsRenderingHelper::GetInstance();
    renderingHelper->GetHTMLRenderer().SetDC(&dc);
    renderingHelper->GetHTMLRenderer().SetStandardFonts( 8 );

    //Prepare HTML texts
    std::string str1 = "<FONT color="+ToString(wxColour(textR, textG, textB).GetAsString(wxC2S_HTML_SYNTAX).mb_str())+">"+renderingHelper->GetHTMLText(com1)+"</FONT>";
    std::string str2 = "<FONT color="+ToString(wxColour(textR, textG, textB).GetAsString(wxC2S_HTML_SYNTAX).mb_str())+">"+renderingHelper->GetHTMLText(com2)+"</FONT>";

    //Calculate space constraints
    const int sideSeparation = 3; //Spacing between text and borders

    unsigned int textWidth = com2.empty() ? width-sideSeparation*2 : width/2-sideSeparation*2;
    renderingHelper->GetHTMLRenderer().SetSize(textWidth, 9999);

    renderingHelper->GetHTMLRenderer().SetHtmlText(str1);
    unsigned int text1Height = renderingHelper->GetHTMLRenderer().GetTotalHeight();

    renderingHelper->GetHTMLRenderer().SetHtmlText(str2);
    unsigned int text2Height = renderingHelper->GetHTMLRenderer().GetTotalHeight();

    //Prepare background
    dc.SetBrush(wxBrush(wxColour(r, v, b), wxTRANSPARENT));
    dc.SetPen(wxPen(wxColour(r/2, v/2, b/2), 1));

    //Draw the background
    wxRect rectangle(x, y, width-1, text1Height > text2Height ? sideSeparation+text1Height+sideSeparation : sideSeparation+text2Height+sideSeparation);
    dc.GradientFillLinear(rectangle, wxColour(r+20 > 255 ? 255 : r+20, v+20 > 255 ? 255 : v+20, b+20 > 255 ? 255 : b+20), wxColour(r, v, b), wxSOUTH);
    dc.DrawRectangle(rectangle);

    //Draw text
    {
        renderingHelper->GetHTMLRenderer().SetHtmlText(str1);
        wxArrayInt neededArray;
        renderingHelper->GetHTMLRenderer().Render(x + sideSeparation, y+sideSeparation, neededArray);
    }
    if ( !com2.empty() ) //Optional text
    {
        renderingHelper->GetHTMLRenderer().SetHtmlText(str2);
        wxArrayInt neededArray;
        renderingHelper->GetHTMLRenderer().Render(x + sideSeparation + textWidth + sideSeparation, y+sideSeparation, neededArray);
    }
}

unsigned int CommentEvent::GetRenderedHeight(unsigned int width) const
{
    if ( eventHeightNeedUpdate )
    {
        EventsRenderingHelper * renderingHelper = EventsRenderingHelper::GetInstance();

        wxMemoryDC dc;
        wxBitmap fakeBmp(1,1);
        dc.SelectObject(fakeBmp);

        renderingHelper->GetHTMLRenderer().SetDC(&dc);
        renderingHelper->GetHTMLRenderer().SetStandardFonts( 8 );

        std::string str1 = "<FONT color="+ToString(wxColour(textR, textG, textB).GetAsString(wxC2S_HTML_SYNTAX).mb_str())+">"+renderingHelper->GetHTMLText(com1)+"</FONT>";
        std::string str2 = "<FONT color="+ToString(wxColour(textR, textG, textB).GetAsString(wxC2S_HTML_SYNTAX).mb_str())+">"+renderingHelper->GetHTMLText(com2)+"</FONT>";

        //Calculate space constraints
        const int sideSeparation = 3; //Spacing between text and borders

        unsigned int textWidth = com2.empty() ? width-sideSeparation*2 : width/2-sideSeparation*2;
        renderingHelper->GetHTMLRenderer().SetSize(textWidth, 9999);

        renderingHelper->GetHTMLRenderer().SetHtmlText(str1);
        unsigned int text1Height = renderingHelper->GetHTMLRenderer().GetTotalHeight();

        renderingHelper->GetHTMLRenderer().SetHtmlText(str2);
        unsigned int text2Height = renderingHelper->GetHTMLRenderer().GetTotalHeight();

        renderedHeight = text1Height > text2Height ? sideSeparation+text1Height+sideSeparation : sideSeparation+text2Height+sideSeparation;
    }

    return renderedHeight+2;//2 : 2 small borders
}
#endif
