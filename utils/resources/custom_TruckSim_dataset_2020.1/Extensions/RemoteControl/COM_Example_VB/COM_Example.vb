Module COM_Example
    Sub Main()
        Dim h As Object = CreateObject("TruckSim.Application")

        h.GoHome()

        ' Look for dataset "Baseline COM" in category "External Control of Runs".
        Dim templateExists As Boolean = False
        h.DataSetExists("", "Baseline COM", "External Control of Runs", templateExists)
        If Not (templateExists) Then
            Return
        End If

        ' delete new dataset if it already exists to avoid a name conflict
        h.DeleteDataSet("", "New Run Made with COM", "External Control of Runs")

        ' Make a new dataset based on the baseline COM
        h.Gotolibrary("", "Baseline COM", "External Control of Runs")
        h.CreateNew() ' duplicate the baseline and rename
        h.DatasetCategory("New Run Made with COM", "External Control of Runs")

        ' Set up run control to use a different speed as in the Quick Start Guide
        h.Checkbox("#Checkbox8", "1") ' enable checkbox to show more options
        h.Checkbox("#Checkbox3", "1") ' enable checkbox to show driver controls
        h.Ring("#RingCtrl0", "1") ' select constant target speed
        h.Yellow("*SPEED", "150") ' set the target speed

        ' Run the Simulation and view results
        h.Run("", "")
        h.Checkbox("#Checkbox1", "0") ' disable overlays
        h.LaunchPlot()

    End Sub
End Module
