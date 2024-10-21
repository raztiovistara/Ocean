import express from "express";
import fs from "fs/promises";
import path from "path";

const directoryPath = "watched_folder";

const app = express();
const PORT = 3001;

const ANDROID_IP = "192.168.43.1";
const ANDROID_PORT = 8080;

const now = new Date().getTime();

const oneYearMs = 31_536_000_000;

fs.readdir(directoryPath, (err, files) => {
  if (err) {
    return console.error("Unable to scan directory:", err);
  }

  files.forEach((file) => {
    const filePath = path.join(directoryPath, file);

    fs.stat(filePath, (err, stats) => {
      if (err) {
        console.error(`Unable to read file stats for ${file}:`, err);
        return;
      }

      const lastModified = new Date(stats.mtime).getTime();
      const isStale = now - lastModified > oneYearMs;

      console.log(
        `${file} is ${isStale ? "stale" : "fresh"} (Last Modified: ${
          stats.mtime
        })`
      );
    });
  });
});
