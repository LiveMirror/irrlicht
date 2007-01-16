// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUIEDIT_WORKSPACE_H_INCLUDED__
#define __C_GUIEDIT_WORKSPACE_H_INCLUDED__

#include "IGUIElement.h"
#include "CGUIEditWindow.h"

namespace irr
{
namespace gui
{

	//! Adding the GUI Editor Workspace to an element allows you
	/** to create, edit, load and save any elements supported
		by any loaded factories.
		When you add it without a parent (to the root element)
		it will also allow you to edit, load and save settings in
		the current skin.
	*/

	class CGUIEditWorkspace : public IGUIElement
	{
	public:

		//! constructor
		CGUIEditWorkspace(IGUIEnvironment* environment, s32 id=-1, IGUIElement *parent=0);

		//! destructor
		~CGUIEditWorkspace();

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! Removes a child.
		virtual void removeChild(IGUIElement* child);

		//! draws the element and its children
		virtual void draw();

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

		//! Sets the menu command id's
		/** The GUI editor defaults to command ID's from 15639 (0x3D17) to 15639+EGUIEDMC_COUNT
		In the rare case that these are already in use and you wish to use menus 
		while the editor is present you can set a new offset here.
		*/
		virtual void setMenuCommandIDStart(s32 id);

		//! grid drawing...
		virtual void setDrawGrid(bool drawGrid);
		virtual void setGridSize(core::dimension2di	&gridSize);
		virtual void setUseGrid(bool useGrid);

		//! returns the first editable element under the mouse
		virtual IGUIElement* getEditableElementFromPoint(IGUIElement *start, const core::position2di &point, s32 index=0 );

		//! selecting elements
		virtual void setSelectedElement(IGUIElement *sel);
		virtual void selectNextSibling();
		virtual void selectPreviousSibling();

		//! returns the selected element
		virtual IGUIElement* getSelectedElement();

	private:

		bool isMyChild(IGUIElement* target);

		enum EGUIEDIT_MODE
		{
			// when we are currently selecting an element
			EGUIEDM_SELECT=0,
			// selecting a new parent for the selected element
			EGUIEDM_SELECT_NEW_PARENT,

			// moving the selected element
			EGUIEDM_MOVE,
			// resizing the selected element
			EGUIEDM_RESIZE_TL,
			EGUIEDM_RESIZE_T,
			EGUIEDM_RESIZE_TR,
			EGUIEDM_RESIZE_R,
			EGUIEDM_RESIZE_BR,
			EGUIEDM_RESIZE_B,
			EGUIEDM_RESIZE_BL,
			EGUIEDM_RESIZE_L
		};

		enum EGUIEDIT_MENUCOMMANDS
		{
			//! file commands
			EGUIEDMC_FILE_NEW,
			EGUIEDMC_FILE_LOAD,
			EGUIEDMC_FILE_SAVE,
			//! edit menu
			EGUIEDMC_CUT_ELEMENT,
			EGUIEDMC_COPY_ELEMENT,
			EGUIEDMC_PASTE_ELEMENT,
			EGUIEDMC_DELETE_ELEMENT,
			EGUIEDMC_SET_PARENT,
			EGUIEDMC_BRING_TO_FRONT,
			EGUIEDMC_SAVE_ELEMENT,
			//! grid
			EGUIEDMC_TOGGLE_GRID,
			EGUIEDMC_TOGGLE_SNAP_GRID,
			EGUIEDMC_SET_GRID_SIZE,
			EGUIEDMC_TOGGLE_EDITOR,

			EGUIEDMC_INSERT_XML,

			//! number of menu options
			EGUIEDMC_COUNT
		};

		EGUIEDIT_MODE getModeFromPos(core::position2di p);

		EGUIEDIT_MODE		CurrentMode;
		EGUIEDIT_MODE		MouseOverMode;
		core::position2di	DragStart;
		core::position2di	StartMovePos;
		core::rect<s32>		SelectedArea;

		bool				DrawGrid, UseGrid;
		core::dimension2di	GridSize;
		s32					MenuCommandStart;

		IGUIElement			*MouseOverElement, 
							*SelectedElement;
		CGUIEditWindow		*EditorWindow;

		core::stringw		CopyBuffer;
		

	};


} // end namespace gui
} // end namespace irr

#endif

