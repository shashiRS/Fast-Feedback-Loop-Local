# standard Python import area
import os


class Handler:
    """
    This is a class to create a sdl file
    The class creates a sdl file with the initialization function

    external functions:
        Handler(sdl_name, num_group, num_sub_group)
        deleteSdlFile()
    """

    def __init__(self, sdl_name, num_view=1, num_group=1, num_sub_group=1):
        """
        internal initialization function to generate a sdl file for testing

        :param sdl_name: this is the name of the sdl file which will be generated
        :type sdl_name:  string
        :param num_group: number of Groups in the root directory of the signals , defaults to 1
        :type num_group: int, optional
        :param num_sub_group: number of under folder in each Group , defaults to 1
        :type num_sub_group: int, optional
        """

        self.sdl_name = sdl_name + ".sdl"
        self.num_View = num_view
        self.num_Group = num_group
        self.num_SubGroup = num_sub_group

        self.path_sdl = os.path.join(os.path.dirname(os.path.realpath(__file__)), self.sdl_name)

        self.sdl_file = open(self.path_sdl, "w")

        self.__generateSdlFile()

    def __generateSdlFile(self):
        """
        This function generate a sdl file with the settings in the __init__
        """
        whitespace_view = "    "
        whitespace_group = whitespace_view * 2
        whitespace_subproup = whitespace_view * 3

        self.sdl_file.write(
            r'<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema"' + r' ByteAlignment="1" Version="2.0">'
        )
        self.sdl_file.write("\n")
        for v in range(self.num_View):
            self.sdl_file.write(whitespace_view + r'<View Name="View' + str(v + 1) + r'" CycleID="2">')
            self.sdl_file.write("\n")
            for k in range(self.num_Group):
                self.sdl_file.write(
                    whitespace_group
                    + r'<Group Name="Group'
                    + str(k + 1)
                    + r'" Address="90100000"'
                    + r' ArrayLen="1" Size="80">'
                )
                self.sdl_file.write("\n")
                for i in range(self.num_SubGroup):
                    self.sdl_file.write(whitespace_subproup + whitespace_view * i)
                    self.sdl_file.write(
                        r'<SubGroup Name="SubGroup' + str(i + 1) + r'" Offset="4"' + r' ArrayLen="1" Size="12">'
                    )
                    self.sdl_file.write("\n")
                for i in reversed(range(self.num_SubGroup)):
                    self.sdl_file.write(whitespace_subproup + whitespace_view * i)
                    self.sdl_file.write(r"</SubGroup>")
                    self.sdl_file.write("\n")

                self.sdl_file.write(r"		</Group>")
                self.sdl_file.write("\n")
            self.sdl_file.write(r"	</View>")
            self.sdl_file.write("\n")

        self.sdl_file.write(r"</SdlFile>")
        self.sdl_file.write("\n")
        self.sdl_file.close()

    def deleteSdlFile(self):
        """
        external function to delete the sdl file
        """
        self.__deleteSdlFile()

    def __deleteSdlFile(self):
        """
        internal function to delete the sdl file
        """

        try:
            os.remove(self.path_sdl)
            print("File is deleted successfully")
        except OSError as e:
            print(e)
