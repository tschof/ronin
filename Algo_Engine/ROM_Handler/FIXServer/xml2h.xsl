<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="yes" encoding="ISO-8859-1" /> 
	
<xsl:template match="/">
<xsl:apply-templates select="om" />
</xsl:template>
	
<xsl:template match="om">
#ifndef OM_MESSAGES
#define OM_MESSAGES

#include "OMMessage.h"

namespace OM
{
<xsl:apply-templates select="enum"/>
<xsl:apply-templates select="msg"/>
}

#endif
</xsl:template>

<xsl:template match="enum">
	enum <xsl:value-of select="@name"/>Enum
	{
<xsl:apply-templates select="value"/>
	};
</xsl:template>

<xsl:template match="value">
<xsl:text>&#9;&#9;</xsl:text><xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/> = <xsl:value-of select="@const"/>,
</xsl:template>
	
<xsl:template match="msg">
	class <xsl:value-of select="@name"/> : public Message
	{
		public:
			<xsl:value-of select="@name"/>();
			<xsl:value-of select="@name"/>( const Message&amp; message );

<xsl:apply-templates select="field"/>
	
	};	
</xsl:template>
	
<xsl:template match="field">
	<xsl:choose>
		<xsl:when test="@type='string'">
			const std::string&amp; Get<xsl:value-of select="@name"/>() const
				{ return GetField(<xsl:value-of select="@position"/>); } 
			void Set<xsl:value-of select="@name"/>( const std::string&amp; value )
				{ SetField(<xsl:value-of select="@position"/>, value); }
		</xsl:when>
		<xsl:when test="@type='time'">
			time_t Get<xsl:value-of select="@name"/>() const
				{ return ConvertStringToTime( GetField(<xsl:value-of select="@position"/>) ); }
			void Set<xsl:value-of select="@name"/>( time_t value )
				{ SetField( <xsl:value-of select="@position"/>, ConvertTimeToString(value) ); }
		</xsl:when>
		<xsl:when test="@type='int'">
			int Get<xsl:value-of select="@name"/>() const
				{ return ConvertStringToInt( GetField(<xsl:value-of select="@position"/>) ); }
			void Set<xsl:value-of select="@name"/>( int value )
				{ SetField( <xsl:value-of select="@position"/>, ConvertIntToString(value) ); }
		</xsl:when>
		<xsl:when test="@type='double'">
			double Get<xsl:value-of select="@name"/>() const
				{ return ConvertStringToDouble( GetField(<xsl:value-of select="@position"/>) ); }
			void Set<xsl:value-of select="@name"/>( double value )
				{ SetField( <xsl:value-of select="@position"/>, ConvertDoubleToString(value) ); }
		</xsl:when>
		<xsl:when test="@type='char'">
			char Get<xsl:value-of select="@name"/>() const
				{ return ConvertStringToChar( GetField(<xsl:value-of select="@position"/>) ); }
			void Set<xsl:value-of select="@name"/>( char value )
				{ SetField( <xsl:value-of select="@position"/>, ConvertCharToString(value) ); }
		</xsl:when>
	</xsl:choose>
</xsl:template>


</xsl:stylesheet>
