/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ide;

import java.util.ArrayList;
import java.util.List;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent.EventType;
import javax.swing.event.UndoableEditEvent;
import javax.swing.event.UndoableEditListener;
import javax.swing.text.AbstractDocument;
import javax.swing.undo.CannotUndoException;
import javax.swing.undo.UndoManager;
import javax.swing.undo.UndoableEdit;

/**
 *
 * @author ilya
 */
class MyUndoableEditListener implements UndoableEditListener {
    private List<ChangeListener> OpListeners = new ArrayList<>(8);
    private UndoManager undoManager;
    
    public MyUndoableEditListener() {
        undoManager = new UndoManager();
    }
    
    public void addChangeListener(ChangeListener changeListener) {
        OpListeners.add(changeListener);
    }

    public void removeChangeListener(ChangeListener changeListener) {
        OpListeners.remove(changeListener);
    }
    
    @Override
    public void undoableEditHappened(UndoableEditEvent e) {

        UndoableEdit  evt = e.getEdit();
        if (evt instanceof AbstractDocument.DefaultDocumentEvent) {
                AbstractDocument.DefaultDocumentEvent event = (AbstractDocument.DefaultDocumentEvent) evt;
                EventType type = event.getType();
                if (type == EventType.INSERT || type == EventType.REMOVE) {
                    System.out.println("Undoable event happened");
                    undoManager.undoableEditHappened(e);
                    OpStateChanged();
                }
        }        
        //Remember the edit and update the menus
        
    }
    
    protected void OpStateChanged() {
        if (OpListeners.isEmpty()) {
            return;
        }
        ChangeEvent evt = new ChangeEvent(this);
        for (ChangeListener listener : OpListeners) {
            listener.stateChanged(evt);
        }
    }
    
    public void undo() throws CannotUndoException {
        if (!canUndo()) {
            //discardAllEdits();
            throw new CannotUndoException();
            
        }

        undoManager.undo();
        //System.out.println(undoManager);
        OpStateChanged();
    }
    

    public void redo() throws CannotUndoException {
        if (!canRedo()) {
            //discardAllEdits();
            throw new CannotUndoException();
        }

        undoManager.redo();

        OpStateChanged();
    }
    
    public boolean canUndo() {
        return undoManager.canUndo();
    }

    public boolean canRedo() {
        return undoManager.canRedo();
    }
    
    public void discardAllEdits() {
        System.out.println("Discard called");
        undoManager.discardAllEdits();
        OpStateChanged();
    }
} 
