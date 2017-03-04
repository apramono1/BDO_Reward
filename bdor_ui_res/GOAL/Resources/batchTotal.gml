<?xml version="1.0" encoding="UTF-8"?>
<!-- ============================================================ -->
<!-- batchTotal.gml                              -->
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
  <goal name="batchTotal">
    <window>
      <grow>all</grow>
      <expand>all</expand>
      <children>
        <layout>
          <shrink>all</shrink>
          <expand>all</expand>
          <children>
            <label>
              <idname>../widgetMappings.iml#BATCH_TOTAL_HOST_NAME</idname>
              <item>
                <column>0</column>
                <row>0</row>
              </item>
              <shrink>none</shrink>
              <expand>all</expand>
              <textalign>center</textalign>
              <text>Host</text>
            </label>
            <layout>
              <item>
                <column>0</column>
                <row>1</row>
              </item>
              <shrink>all</shrink>
              <expand>all</expand>
              <minsize>
                <width>996</width>
                <height>545</height>
                <unit>perthousand</unit>
              </minsize>
              <children>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_SALES_TOTAL</idname>
                  <item>
                    <column>2</column>
                    <row>1</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>right</textalign>
                  <text>HKD 9999</text>
                </label>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_REFUND_TOTAL</idname>
                  <item>
                    <column>2</column>
                    <row>2</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>right</textalign>
                  <text>HKD 9999</text>
                </label>
                <label>
                  <item>
                    <column>0</column>
                    <row>2</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>left</textalign>
                  <text>REFUND:</text>
                </label>
                <label>
                  <item>
                    <column>0</column>
                    <row>3</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>left</textalign>
                  <text>TOTAL:</text>
                </label>
                <label>
                  <item>
                    <column>0</column>
                    <row>1</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>left</textalign>
                  <text>SALES:</text>
                </label>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_GRAND_TOTAL</idname>
                  <item>
                    <column>2</column>
                    <row>3</row>
                  </item>
                  <shrink>none</shrink>
                  <expand>all</expand>
                  <textalign>right</textalign>
                  <text>HKD 9999</text>
                </label>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_SALE_COUNT</idname>
                  <item>
                    <column>1</column>
                    <row>1</row>
                  </item>
                  <expand>all</expand>
                  <textalign>center</textalign>
                  <text>COUNT</text>
                </label>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_REFUND_COUNT</idname>
                  <item>
                    <column>1</column>
                    <row>2</row>
                  </item>
                  <expand>all</expand>
                  <textalign>center</textalign>
                  <text>COUNT</text>
                </label>
                <label>
                  <idname>../widgetMappings.iml#BATCH_TOTAL_TOTAL_COUNT</idname>
                  <item>
                    <column>1</column>
                    <row>3</row>
                  </item>
                  <textalign>center</textalign>
                  <text>COUNT</text>
                </label>
              </children>
            </layout>
            <label>
              <item>
                <column>0</column>
                <row>2</row>
              </item>
              <shrink>none</shrink>
              <expand>all</expand>
              <textalign>center</textalign>
              <text>Press [F] to print Report</text>
            </label>
          </children>
        </layout>
        <timer>
          <idname>../widgetMappings.iml#BATCH_TOTAL_TIMEOUT_TIMER</idname>
        </timer>
      </children>
    </window>
  </goal>
</DAL>