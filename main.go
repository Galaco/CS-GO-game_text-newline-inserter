

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"github.com/galaco/bsp"
	"strings"
)

func CreateBackup(target string) {
	os.Link(target, target + ".bak")
}

func OpenBsp(target string) *os.File{
	file,err := os.Open(target)
	if err != nil {
		log.Fatal(err)
	}

	return file
}

func UpdateEntdataLump(entdata string, placeholder string) string {
	n := strings.Count(entdata, placeholder)
	fmt.Printf("Found %d instances to replace\n", n)
	return strings.Replace(entdata, placeholder, "\n", -1)
}

func main() {
	//intro
	fmt.Println("Galaco/DormantLemon's CS:GO game_text newline inserter")
	fmt.Print("\n\n")

	//assert valid parameters
	target := flag.String("target", "", ".bsp to fix characters on")
	placeholder := flag.String("placeholder", "", "character to replace with newlines")
	flag.Parse()

	fmt.Println(*target)
	fmt.Println(*placeholder)
	if *target == "" || len(*placeholder) != 1 {
		log.Fatal("Invalid arguments have been provided. target must be a valid bsp, placeholder must be a single character")
	}

	//Start with a backup
	CreateBackup(*target)
	file := OpenBsp(*target)

	//Read bsp file
	bspFile := bsp.Parse(file)
	file.Close()

	// Fetch and update entdata payload
	entdata := bspFile.Lumps[0].GetData().(string)
	payload := UpdateEntdataLump(entdata, *placeholder)
	bspFile.Lumps[0] = bspFile.Lumps[0].FromBytes([]byte(payload), int32(len(entdata)))

	//Export bsp back to file
	output := bsp.ToBytes(bspFile)

	outFile,err := os.Create(*target)
	if err != nil {
		log.Fatal(err)
	}
	_,err = outFile.Write(output)
	if err != nil {
		log.Fatal(err)
	}

	outFile.Sync()


	n := strings.Count(payload, *placeholder)
	if n != 0 {
		fmt.Printf("Failed to replace all instances. %d remain.", n)
	} else {
		fmt.Println("Successfully replaced all placeholders.")
	}
}

