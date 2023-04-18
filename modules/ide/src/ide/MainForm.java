/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ide;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author mipt
 */
public class MainForm extends javax.swing.JFrame {
    
    public class InputQueue {
      Queue<String> queue;
      public InputQueue() {
        queue = new LinkedList<String>();
      } 
      public synchronized void clear() {
        queue.clear();
      }
      public synchronized void add(String str) {
        queue.add(str);
      } 
      public synchronized String poll() {
        return queue.poll();
      }
    }
    InputQueue inputQueue;
    public static File curFile = null;
    
    //output message to "IDE terminal" in format <prefix>+<message>
    private synchronized void log(String prefix, String message) {
      if (message == null)
        return;
      textAreaOutput.setText(textAreaOutput.getText() + prefix + message);
    }
    //output message to "IDE terminal" in format <prefix>+<message>+'\n'
    private synchronized void logln(String prefix, String message) {
      if (message == null)
        return;
      log(prefix, message + '\n');
    }
    //output text to "IDE editor" in format <message>
    private synchronized void output(String text) {
      if (text == null)
        return;
      textAreaInput.append(text);
    }
    //output text to "IDE editor" in format <message>+'\n'
    private synchronized void outputln(String text) {
      output(text + '\n');
    }
    //clear "IDE editor"
    private synchronized void outputClear() {
      textAreaInput.setText("");
    }
    //return text from editor
    private synchronized String input() {
      return textAreaInput.getText();
    }
    //read File and output it to text editor
    private boolean downloadFile(File iFile) {
      if (iFile == null)
        return false;
      if (!iFile.exists()) {
        System.out.println("File " + iFile + " does not exist");
        logln("Error: ", "File " + iFile + " does not exist");
        return false;
      }
      if(!iFile.canRead()) {
        System.out.println("File " + iFile + " can not be read");
        logln("Error: ", "File " + iFile + " can not be read");
        return false;
      }
      try (FileReader reader = new FileReader(iFile)) {
        char[] buf = new char[256];
        int c;
        while((c = reader.read(buf))>0){  
          if(c < 256) {
            buf = Arrays.copyOf(buf, c);
            output(new String(buf));
          }
        }
      }
      catch (Exception ex) {
         System.out.println(ex);
         logln("Error: ", "Promblems with file " + iFile + " reading");
         return false;
      }
      System.out.println("menu->file->open->" + iFile);
      logln("Message: ", "File " + iFile + " sucessfully downlad");  
      return true;
    }
    //upload text ftom text editor to File
    private boolean uploadFile(File oFile) {
      if (oFile == null)
        return false;
      if (!oFile.exists()) {
        System.out.println("File " + oFile + " does not exist");
        logln("Error: ", "File " + oFile + " does not exist");
        return false;
      }
      if(!oFile.canWrite()) {
        System.out.println("File " + oFile + " can not be written");
        logln("Error: ", "File " + oFile + " can not be written");
        return false;
      }
      try (FileWriter writer = new FileWriter(oFile)) {
        writer.write(input());
      }
      catch (Exception ex) {
         System.out.println(ex);
         logln("Error: ", "Promblems with file " + oFile + " writing");
         return false;
      }
      System.out.println("menu->file->save->" + oFile);
      logln("Message: ", "File " + oFile + " sucessfully saved");  
      return true;
    }
    //create file by its oFile
    private boolean createFile(File oFile) {
      if (oFile == null)
        return false;
      try {
        boolean created =  oFile.createNewFile();
        if (created) {
          System.out.println("menu->file->saveas->" + oFile);
          logln("Message: ", "File " + oFile + " sucessfully created"); 
          return true;
        }
      }
      catch (IOException ex) {
        System.out.println(ex);
      }
      logln("Error: ", "Promblems with file " + oFile + " creating");
      return false;
    }
    //compile String code with ParaCL
    //FIX: does not work
    private void compileCode(String code) throws IOException, InterruptedException {
      if (curFile == null) {
        logln("Error: ", "Save code to file for execution");
        return;
      }
      uploadFile(curFile);
      List<String> command = new ArrayList<String>();
      command.add("/home/mipt/ParaCL/modules/bison/test.out");
      command.add(curFile.toString());
      command.add("--build");
      ProcessBuilder processBuilder = new ProcessBuilder(command);
      Process process = processBuilder.start();
      BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
      BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
      String line;
      while (process.isAlive()) {
        while (reader.ready()) {
         if ((line = reader.readLine()) != null) {
           logln("> ", line);
           System.out.println(line);
          }
        }
        while (errorReader.ready()) {
          if ((line = errorReader.readLine()) != null) {
            logln("> ", line);
            System.out.println(line);
          }
         }
        Thread.sleep(10);
      }
      while (reader.ready()) {
         if ((line = reader.readLine()) != null) {
           logln("> ", line);
           System.out.println(line);
          }
        }
      while (errorReader.ready()) {
        if ((line = errorReader.readLine()) != null) {
          logln("> ", line);
          System.out.println(line);
        }
      }
      reader.close();
      errorReader.close();
    }
    //compile String code with ParaCL
    private void executeCode() throws Exception {
      if (curFile == null) {
        logln("Error: ", "Save code to file for execution");
        return;
      }
      uploadFile(curFile);
      List<String> command = new ArrayList<String>();
      command.add("/home/mipt/ParaCL/modules/bison/test.out");
      command.add(curFile.toString());
      ProcessBuilder processBuilder = new ProcessBuilder(command);
      Process process = processBuilder.start();
      
      BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
      BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
      BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
      inputQueue.clear();
      String line;
      while (process.isAlive()) {
        while (reader.ready()) {
         if ((line = reader.readLine()) != null) {
           logln("> ", line);
           System.out.println(line);
          }
        }
        while (errorReader.ready()) {
          if ((line = errorReader.readLine()) != null) {
            logln("> ", line);
            System.out.println(line);
          }
         } 
        while ((line = inputQueue.poll()) != null) {
          writer.write(line);
          writer.newLine(); 
        }
        writer.flush();
        
        Thread.sleep(10);
      }
      while (reader.ready()) {
         if ((line = reader.readLine()) != null) {
           logln("> ", line);
           System.out.println(line);
          }
        }
      while (errorReader.ready()) {
        if ((line = errorReader.readLine()) != null) {
          logln("> ", line);
          System.out.println(line);
        }
      }
      reader.close();
      errorReader.close();
    }
    /** Creates new form MainForm */
    public MainForm() {
        inputQueue = new InputQueue();
        initComponents();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents()
  {

    jFileChooser = new javax.swing.JFileChooser();
    jSplitPane = new javax.swing.JSplitPane();
    jSplitPane1 = new javax.swing.JSplitPane();
    textFieldInput = new java.awt.TextField();
    textAreaOutput = new java.awt.TextArea();
    textAreaInput = new java.awt.TextArea();
    jMenuBar = new javax.swing.JMenuBar();
    jMenuFile = new javax.swing.JMenu();
    jMenuItemFileOpen = new javax.swing.JMenuItem();
    jMenuItemFileSave = new javax.swing.JMenuItem();
    jMenuItemFileSaveAs = new javax.swing.JMenuItem();
    jMenuView = new javax.swing.JMenu();
    jMenuViewFont = new javax.swing.JMenuItem();
    jMenuViewBackground = new javax.swing.JMenuItem();
    jMenuRun = new javax.swing.JMenu();
    jMenuRunBuilt = new javax.swing.JMenuItem();
    jMenuRunExecute = new javax.swing.JMenuItem();

    jFileChooser.setToolTipText("");
    jFileChooser.setName(""); // NOI18N

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setMinimumSize(new java.awt.Dimension(204, 204));

    jSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
    jSplitPane.setResizeWeight(0.8);
    jSplitPane.setToolTipText("");

    jSplitPane1.setDividerSize(8);
    jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
    jSplitPane1.setResizeWeight(1.0);
    jSplitPane1.setToolTipText("");
    jSplitPane1.setName(""); // NOI18N
    jSplitPane1.setPreferredSize(new java.awt.Dimension(102, 102));
    jSplitPane1.setRequestFocusEnabled(false);

    textFieldInput.addKeyListener(new java.awt.event.KeyAdapter()
    {
      public void keyPressed(java.awt.event.KeyEvent evt)
      {
        textFieldInputKeyPressed(evt);
      }
    });
    jSplitPane1.setBottomComponent(textFieldInput);

    textAreaOutput.setEditable(false);
    jSplitPane1.setTopComponent(textAreaOutput);

    jSplitPane.setRightComponent(jSplitPane1);

    textAreaInput.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
    textAreaInput.setText("Attention: view menu does not work\n");
    jSplitPane.setLeftComponent(textAreaInput);

    getContentPane().add(jSplitPane, java.awt.BorderLayout.CENTER);

    jMenuFile.setText("File");

    jMenuItemFileOpen.setText("Open");
    jMenuItemFileOpen.setToolTipText("");
    jMenuItemFileOpen.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuItemFileOpenActionPerformed(evt);
      }
    });
    jMenuFile.add(jMenuItemFileOpen);

    jMenuItemFileSave.setText("Save");
    jMenuItemFileSave.setToolTipText("");
    jMenuItemFileSave.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuItemFileSaveActionPerformed(evt);
      }
    });
    jMenuFile.add(jMenuItemFileSave);

    jMenuItemFileSaveAs.setText("Save as");
    jMenuItemFileSaveAs.setToolTipText("");
    jMenuItemFileSaveAs.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuItemFileSaveAsActionPerformed(evt);
      }
    });
    jMenuFile.add(jMenuItemFileSaveAs);

    jMenuBar.add(jMenuFile);

    jMenuView.setText("View");

    jMenuViewFont.setText("Font");
    jMenuView.add(jMenuViewFont);

    jMenuViewBackground.setText("Background");
    jMenuView.add(jMenuViewBackground);

    jMenuBar.add(jMenuView);

    jMenuRun.setText("Run");

    jMenuRunBuilt.setText("Build");
    jMenuRunBuilt.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuRunBuiltActionPerformed(evt);
      }
    });
    jMenuRun.add(jMenuRunBuilt);

    jMenuRunExecute.setText("Execute");
    jMenuRunExecute.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuRunExecuteActionPerformed(evt);
      }
    });
    jMenuRun.add(jMenuRunExecute);

    jMenuBar.add(jMenuRun);

    setJMenuBar(jMenuBar);

    pack();
  }// </editor-fold>//GEN-END:initComponents
//action on menu item Open that opens and reads the file
  private void jMenuItemFileOpenActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileOpenActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileOpenActionPerformed
    jFileChooser.setSelectedFile(new File(""));
    jFileChooser.showOpenDialog(this);
    File iFile = jFileChooser.getSelectedFile();
    String reserve = input();
    outputClear();
    if (downloadFile(iFile))
      curFile = iFile;
    else
      output(reserve);
  }//GEN-LAST:event_jMenuItemFileOpenActionPerformed

  private void jMenuItemFileSaveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileSaveActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileSaveActionPerformed
    uploadFile(curFile);
  }//GEN-LAST:event_jMenuItemFileSaveActionPerformed

  private void jMenuItemFileSaveAsActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileSaveAsActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileSaveAsActionPerformed
    jFileChooser.setSelectedFile(new File(""));
    jFileChooser.showSaveDialog(this);
    File oFile = jFileChooser.getSelectedFile();
    if (createFile(oFile))
      curFile = oFile;
    else
      return;
    uploadFile(curFile);
  }//GEN-LAST:event_jMenuItemFileSaveAsActionPerformed

  private void jMenuRunBuiltActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuRunBuiltActionPerformed
  {//GEN-HEADEREND:event_jMenuRunBuiltActionPerformed
    System.out.println("menu->run->build");
    Thread compiler = new Thread(() ->
    {
      try {
        compileCode(input());
      }
      catch (IOException | InterruptedException ex) {
        System.out.println(ex);
        logln("Error: ", "Compilation fault");
      }
    });
    compiler.start();
  }//GEN-LAST:event_jMenuRunBuiltActionPerformed

  private void textFieldInputKeyPressed(java.awt.event.KeyEvent evt)//GEN-FIRST:event_textFieldInputKeyPressed
  {//GEN-HEADEREND:event_textFieldInputKeyPressed
    if (evt.getKeyChar() != '\n')
      return;
    System.out.println("Input->Enter");
    logln("< ", textFieldInput.getText());
    inputQueue.add(textFieldInput.getText());
    textFieldInput.setText(" ");
    
  }//GEN-LAST:event_textFieldInputKeyPressed

  private void jMenuRunExecuteActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuRunExecuteActionPerformed
  {//GEN-HEADEREND:event_jMenuRunExecuteActionPerformed
    System.out.println("menu->run->execute");
    Thread executer = new Thread(() ->
    {
      try {
        executeCode();
      }
      catch (Exception ex) {
        System.out.println(ex);
        logln("Error: ", "Execution fault");
      }
    });
    
    executer.start();
    
  }//GEN-LAST:event_jMenuRunExecuteActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>
       
    }

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JFileChooser jFileChooser;
  private javax.swing.JMenuBar jMenuBar;
  private javax.swing.JMenu jMenuFile;
  private javax.swing.JMenuItem jMenuItemFileOpen;
  private javax.swing.JMenuItem jMenuItemFileSave;
  private javax.swing.JMenuItem jMenuItemFileSaveAs;
  private javax.swing.JMenu jMenuRun;
  private javax.swing.JMenuItem jMenuRunBuilt;
  private javax.swing.JMenuItem jMenuRunExecute;
  private javax.swing.JMenu jMenuView;
  private javax.swing.JMenuItem jMenuViewBackground;
  private javax.swing.JMenuItem jMenuViewFont;
  private javax.swing.JSplitPane jSplitPane;
  private javax.swing.JSplitPane jSplitPane1;
  private java.awt.TextArea textAreaInput;
  private java.awt.TextArea textAreaOutput;
  private java.awt.TextField textFieldInput;
  // End of variables declaration//GEN-END:variables

}
