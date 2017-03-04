<?xml version="1.0" encoding="UTF-8"?>
<!-- ============================================================ -->
<!-- testHostScreen.gml                              -->
<!-- ============================================================ -->
<!-- ============================================================ -->
<!-- NOTE: You can optionally associate an "<idname>" element to  -->
<!-- any of the widgets.                                          -->
<!-- The "<idname>" element is required if the application        -->
<!-- business part (written in "C" code) needs to interact with   -->
<!-- the widget.                                                  -->
<!-- The value inside the "<idname>" element must be one of the   -->
<!-- "idname" attributes declared in the "widgetMappings.iml"     -->
<!-- file (located at project root). Please complete this file    -->
<!-- if needed.                                                   -->
<!-- The uniqueness of each <idname> is automatically checked     -->
<!-- (in the editor and at build time).                           -->
<!-- ============================================================ -->
<DAL xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://www.ingenico.com/schemas/dal/1_0">
  <version DAL_version="01.00" this_version="01.00"/>
  <goal name="testHostScreen">
    <window>
      <children>
        <layout>
          <children>
            <label>
              <idname>../widgetMappings.iml#TEST_HOST__PROGRESS_MESSAGE</idname>
              <grow>width</grow>
              <shrink>height</shrink>
              <expand>width</expand>
              <forealign>left</forealign>
              <textalign>left</textalign>
              <paddings>
                <left>5</left>
                <top>0</top>
                <right>0</right>
                <bottom>0</bottom>
                <unit>%</unit>
              </paddings>
              <text>Testing host: 01/20</text>
            </label>
            <label>
              <idname>../widgetMappings.iml#TEST_HOST__NB_ERROR_MESSAGE</idname>
              <item>
                <column>0</column>
                <row>1</row>
              </item>
              <grow>width</grow>
              <shrink>height</shrink>
              <expand>width</expand>
              <forealign>left</forealign>
              <textalign>left</textalign>
              <paddings>
                <left>5</left>
                <top>0</top>
                <right>0</right>
                <bottom>0</bottom>
                <unit>%</unit>
              </paddings>
              <text>Errors: 0</text>
            </label>
          </children>
        </layout>
      </children>
    </window>
  </goal>
</DAL>