#ifndef __view_layout_constants_h__
#define __view_layout_constants_h__

// This file contains some constants we use to implement our standard panel
// layout.
// see: spec 21/4

// Left or right margin.
const int kPanelHorizMargin = 13;

// Top or bottom margin.
const int kPanelVertMargin = 13;

// If some UI has some sub UI. Indent horizontally by the following value.
const int kPanelHorizIndentation = 24;

// When several controls are aligned vertically, the baseline should be spaced
// by the following number of pixels.
const int kPanelVerticalSpacing = 32;

// Vertical spacing between sub UI.
const int kPanelSubVerticalSpacing = 24;

// Vertical spacing between a label and some control.
const int kLabelToControlVerticalSpacing = 8;

// Small horizontal spacing between controls that are logically related.
const int kRelatedControlSmallHorizontalSpacing = 8;

// Horizontal spacing between controls that are logically related.
const int kRelatedControlHorizontalSpacing = 8;

// Vertical spacing between controls that are logically related.
const int kRelatedControlVerticalSpacing = 8;

// Small vertical spacing between controls that are logically related.
const int kRelatedControlSmallVerticalSpacing = 4;

// Horizontal spacing between controls that are logically unrelated.
const int kUnrelatedControlHorizontalSpacing = 12;

// Larger horizontal spacing between unrelated controls.
const int kUnrelatedControlLargeHorizontalSpacing = 20;

// Vertical spacing between controls that are logically unrelated.
const int kUnrelatedControlVerticalSpacing = 20;

// Larger vertical spacing between unrelated controls.
const int kUnrelatedControlLargeVerticalSpacing = 30;

// Vertical spacing between the edge of the window and the
// top or bottom of a button.
const int kButtonVEdgeMargin = 6;

// Vertical spacing between the edge of the window and the
// left or right of a button.
const int kButtonHEdgeMargin = 7;

// Horizontal spacing between buttons that are logically related.
const int kRelatedButtonHSpacing = 6;

#endif //__view_layout_constants_h__

