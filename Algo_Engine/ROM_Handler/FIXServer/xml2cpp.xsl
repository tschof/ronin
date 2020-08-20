<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="yes" encoding="ISO-8859-1" /> 
	
<xsl:template match="/">
<xsl:apply-templates select="om" />
</xsl:template>
	
<xsl:template match="om">
#include "stdafx.h"
#include "OMMessages.h"
#include &lt;time.h&gt;
namespace OM
{
<xsl:apply-templates select="msg"/>
}

</xsl:template>
	
<xsl:template match="msg">
<xsl:text>&#9;</xsl:text><xsl:value-of select="@name"/>::<xsl:value-of select="@name"/>()
		: Message( '<xsl:value-of select="@command"/>', <xsl:value-of select="@length"/> )
	{
<xsl:if test="field[1]/@name='Timestamp'">
		time_t t;
		time(&amp;t);
		SetTimestamp(t);
</xsl:if>
	}

<xsl:text>&#9;</xsl:text><xsl:value-of select="@name"/>::<xsl:value-of select="@name"/>( const Message&amp; message )
		: Message( message )
	{

	}

</xsl:template>
	
<xsl:template match="field">
	
</xsl:template>
</xsl:stylesheet>
