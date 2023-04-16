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
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author mipt
 */
public class MainForm extends javax.swing.JFrame {

    public static File curFile = null;
    
    //output message to "IDE terminal" in format <prefix>+<message>
    private synchronized void log(String prefix, String message) {
      textAreaOutput.setText(textAreaOutput.getText() + prefix + message);
    }
    //output message to "IDE terminal" in format <prefix>+<message>+'\n'
    private synchronized void logln(String prefix, String message) {
      log(prefix, message + '\n');
    }
    //output text to "IDE editor" in format <message>
    private synchronized void output(String text) {
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
    private void compileCode(String code) throws IOException, InterruptedException {
      ProcessBuilder processBuilder = new ProcessBuilder("/home/mipt/ParaCL/modules/bison/test.out");
      Process process = processBuilder.start();
      BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
      BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
      BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
      writer.write(code);
      writer.close();    
      while (process.isAlive())
        Thread.sleep(10);
      String line = "";
      if (reader.ready())
        while ((line = reader.readLine()) != null) {
          logln("", line);
          System.out.println(line);
       }
      if (errorReader.ready())
        while ((line = errorReader.readLine()) != null) {
          logln("", line);
          System.out.println(line);
        }      
      reader.close();
      errorReader.close();
    }
    /** Creates new form MainForm */
    public MainForm() {
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

    jSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
    jSplitPane.setResizeWeight(0.8);

    textAreaOutput.setEditable(false);
    textAreaOutput.setText("Attention: in current version only file options and run->build are working.\n");
    jSplitPane.setBottomComponent(textAreaOutput);

    textAreaInput.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
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

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new MainForm().setVisible(true);
            }
        });
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
  private java.awt.TextArea textAreaInput;
  private java.awt.TextArea textAreaOutput;
  // End of variables declaration//GEN-END:variables

}
